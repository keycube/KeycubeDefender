// Fill out your copyright notice in the Description page of Project Settings.


#include "KCD_Cube.h"

#include "AsyncTreeDifferences.h"
#include "KCD_GameMode.h"
#include "KCD_PlayerController.h"
#include "KCD_Spawner.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AKCD_Cube::AKCD_Cube()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//Instantiate the object component
	CubeMesh = CreateDefaultSubobject<UStaticMeshComponent>("CubeMesh");
	RootComponent = CubeMesh;
}

// Called when the game starts or when spawned
void AKCD_Cube::BeginPlay()
{
	Super::BeginPlay();

	//Get the current player controller
	AKCD_PlayerController* PlayerController = Cast<AKCD_PlayerController>(UGameplayStatics::GetPlayerController(
		this, 0));

	//Subscribes to the delegate for then a key is pressed and when it's released
	PlayerController->KeyPressDelegate.AddDynamic(this, &AKCD_Cube::KeyPress);
	PlayerController->KeyReleaseDelegate.AddDynamic(this, &AKCD_Cube::KeyRelease);

	//Get the current game mode and cast it to the required game mode
	GameModeInstance = Cast<AKCD_GameMode>(UGameplayStatics::GetGameMode(this));

	if(!GameModeInstance->IsValidLowLevel())
	{
		UE_LOG(LogTemp, Warning, TEXT("Game mode is invalid"));
	}
	
	FillKeyMap();
}

void AKCD_Cube::FillKeyMap()
{
	//List of all the child components of the object
	TArray<USceneComponent*> ChildComponents;
	RootComponent->GetChildrenComponents(true, ChildComponents);
	
	// Check all the object's childComponent. If they are of the AKCD_Keys class, we add them
	// to the map and associate them with the right key
	for (auto Child : ChildComponents)
	{
		if(auto ChildActor = Cast<UChildActorComponent>(Child))
		{
			if(auto ChildKey = Cast<AKCD_Keys>(ChildActor->GetChildActor()))
			{
				if(!Keys.Find(ChildKey->AssociatedKey))
				{
					Keys.Add(ChildKey->AssociatedKey, ChildKey); 
				} else
				{
					UE_LOG(LogTemp, Warning, TEXT("Key : %s Was already present"), *ChildKey->AssociatedKey.GetDisplayName().ToString());
				}
				
			}
		}
	}
}

void AKCD_Cube::KeyPress(FKey key)
{
	//TODO : Do this only once (not in the begin play since it isn't instanced yet)
	AKCD_Spawner* SpawnerInstance = GameModeInstance->GetShipSpawner();

	
	if(!Keys.Contains(key))
	{
		return;
	}
	
	AKCD_Keys* KeyPressed = Keys.FindRef(key);

	KeyPressed->KeyPressed_Keys();
	//If the target is invalid, we try to get a new one
	if(!CurrentTarget->IsValidLowLevel())
	{
		if(!SpawnerInstance->IsValidLowLevel())
		{
			UE_LOG(LogTemp, Warning, TEXT("Spawner is invalid"));
			return;
		} 
		
		//If we can't get a new target, we stop
		if(!NewTarget(SpawnerInstance->GetClosestShip(key.GetFName())))
		{
			return;
		}
		 	
	}

	//Try to hit the current target
	if(!CurrentTarget->Hit(key.GetFName()))
	{
		UpdateMultiplicator(false);
	} else
	{
		UpdateMultiplicator(true);
		Score += 1 * ScoreMultiplicator;
	}
	
	OnScoreUpdateDelegate.Broadcast();
}

void AKCD_Cube::KeyRelease(FKey key)
{
	if(!Keys.Contains(key))
	{
		return;
	}
	
	AKCD_Keys* KeyPressed = Keys.FindRef(key);

	KeyPressed->KeyReleased_Keys();
}

void AKCD_Cube::LooseCurrentTarget(AKCD_Ship* ship)
{
	ship->OnShipDestroyedDelegate.RemoveAll(this);
	CurrentTarget = nullptr;
}

void AKCD_Cube::ShipDestroyed(AKCD_Ship* ship)
{
	Score += ship->Reward * ScoreMultiplicator;
	OnScoreUpdateDelegate.Broadcast();
}

void AKCD_Cube::HighlightKeys(TArray<FKey> keysToHighlight)
{
	for (auto key : keysToHighlight)
	{
		
	}
}

void AKCD_Cube::UpdateMultiplicator(bool Success)
{
	if(Success)
	{
		MultiplicatorGauge++;
		if(MultiplicatorGauge % (10 * ScoreMultiplicator) == 0)
		{
			MultiplicatorGauge = 0;
			ScoreMultiplicator += 1;
		}
	} else
	{
		MultiplicatorGauge = 0;
		ScoreMultiplicator = 1;
	}
}

bool AKCD_Cube::NewTarget(AKCD_Ship* ship)
{
	if(ship == nullptr)
		return false;

	//Subscribe to the ShipDestroyed delegate
	ship->OnShipDestroyedDelegate.AddDynamic(this, &AKCD_Cube::LooseCurrentTarget);
	
	CurrentTarget = ship;

	//Give visual feedback of the current target
	CurrentTarget->Targeted();
	return true;
}

AKCD_Ship* AKCD_Cube::GetCurrentTarget()
{
	return CurrentTarget;
}

