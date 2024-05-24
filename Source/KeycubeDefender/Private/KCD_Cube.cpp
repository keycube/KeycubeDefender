// Fill out your copyright notice in the Description page of Project Settings.


#include "KCD_Cube.h"

#include "AsyncTreeDifferences.h"
#include "KCD_GameMode.h"
#include "KCD_PlayerController.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AKCD_Cube::AKCD_Cube()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CubeMesh = CreateDefaultSubobject<UStaticMeshComponent>("CubeMesh");
	RootComponent = CubeMesh;
}

// Called when the game starts or when spawned
void AKCD_Cube::BeginPlay()
{
	Super::BeginPlay();

	AKCD_PlayerController* PlayerController = Cast<AKCD_PlayerController>(UGameplayStatics::GetPlayerController(
		this, 0));

	PlayerController->KeyPressDelegate.AddDynamic(this, &AKCD_Cube::KeyPress);
	PlayerController->KeyReleaseDelegate.AddDynamic(this, &AKCD_Cube::KeyRelease);

	GameModeInstance = Cast<AKCD_GameMode>(UGameplayStatics::GetGameMode(this));

	if(!GameModeInstance->IsValidLowLevel())
	{
		UE_LOG(LogTemp, Warning, TEXT("Game mode is invalid"));
	}
	FillKeyMap();
}

void AKCD_Cube::FillKeyMap()
{
	TArray<USceneComponent*> ChildComponents;
	RootComponent->GetChildrenComponents(true, ChildComponents);

	UE_LOG(LogTemp, Warning, TEXT("Started map fill"));
	
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
	AKCD_Spawner* SpawnerInstance = GameModeInstance->GetShipSpawner();
	if(!Keys.Contains(key))
	{
		return;
	}
	
	AKCD_Keys* KeyPressed = Keys.FindRef(key);

	KeyPressed->KeyPressed_Keys();
	
	if(!CurrentTarget->IsValidLowLevel())
	{
		UE_LOG(LogTemp, Warning, TEXT("CurrentTarget is invalid"));
		if(!SpawnerInstance->IsValidLowLevel())
		{
			UE_LOG(LogTemp, Warning, TEXT("Spawner is invalid"));
			return;
		} 
		
		
		 if(!NewTarget(SpawnerInstance->GetClosestShip(key.GetFName())))
		 {
		 	return;
		 }
		 	
	}

	
	if(!CurrentTarget->Hit(key.GetFName()))
	{
		//TODO : MAKE WRONG LETTER TYPED LOGIC
	}

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

void AKCD_Cube::HighlightKeys(TArray<FKey> keysToHighlight)
{
	for (auto key : keysToHighlight)
	{
		
	}
}

bool AKCD_Cube::NewTarget(AKCD_Ship* ship)
{
	if(ship == nullptr)
		return false;

	ship->OnShipDestroyedDelegate.AddDynamic(this, &AKCD_Cube::LooseCurrentTarget);
	
	CurrentTarget = ship;

	UE_LOG(LogTemp, Warning, TEXT("Targetting ship"));
	CurrentTarget->Targeted();
	return true;
}

AKCD_Ship* AKCD_Cube::GetCurrentTarget()
{
	return CurrentTarget;
}

