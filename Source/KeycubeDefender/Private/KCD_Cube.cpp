// Fill out your copyright notice in the Description page of Project Settings.


#include "KCD_Cube.h"
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

	FTimerHandle TimerHandleGamemodeRefs;

	// Fetch the references set in the gamemode after a delay
	//to allow it to finish fetching them
	GetWorld()->GetTimerManager().SetTimer(TimerHandleGamemodeRefs, [&]()
	{
		SpawnerInstance = GameModeInstance->GetShipSpawner();
		if(SpawnerInstance == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("SpawnerInstance is invalid"));
		}
	},  0.1, false);
	
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
	if(!Keys.Contains(key))
	{
		return;
	}
	
	AKCD_Keys* KeyPressed = Keys.FindRef(key);

	UGameplayStatics::PlaySoundAtLocation(GetWorld(), TypeSound, GetTransform().GetLocation());

	KeyPressed->KeyPressed_Keys();
	//If the target is invalid, we try to get a new one
	NewTargets(Cast<AKCD_Spawner>(SpawnerInstance)->GetClosestShips(key.GetFName()));
	
	if(CurrentTargets.IsEmpty())
		return;

	TArray<AKCD_Ship*> TargetsToRemove;
	//New list to avoid error when removing target during iteration
	TArray<AKCD_Ship*> TargetsToCheck = CurrentTargets;
	
	bool isHitSuccess = false;
	//Try to hit the current targets
	for (auto Ship : TargetsToCheck)
	{
		if(!Ship->Hit(key.GetFName()))
		{
			TargetsToRemove.Add(Ship);
		} else
		{
			isHitSuccess = true;
			UpdateMultiplicator(true);
			Score += 1 * (ScoreMultiplicator * ( 1 + ComboCounter));
		}
	}

	if(!TargetsToRemove.IsEmpty())
	{
		for (auto ToRemove : TargetsToRemove)
		{
			ToRemove->Untargeted();
			RemoveTarget(ToRemove);
		}
	}
	
	if(!isHitSuccess)
	{
		UpdateMultiplicator(false);
		CurrentTargets.Empty();
	}
	
	UpdateHighlight();
	
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

void AKCD_Cube::RemoveTarget(AKCD_Ship* Ship)
{
	CurrentTargets.Remove(Ship);
	Ship->OnShipDestroyedDelegate.RemoveAll(this);
	if(CurrentTargets.IsEmpty())
	{
		ComboCounter = 0;
	}
}

TArray<FKey> AKCD_Cube::TranslateKeys(TArray<FName> KeysToTranslate)
{
	TArray<FKey> translatedKeys;

	for (auto key : KeysToTranslate)
	{
		translatedKeys.Add(FKey(key));
	}
	
	return translatedKeys;
}

void AKCD_Cube::UpdateHighlight()
{
	TArray<FName> targetableLetters;
	if(CurrentTargets.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("Target list is empty"));
		for (auto ship : SpawnerInstance->ShipsAlive)
		{
			targetableLetters.AddUnique(ship->CurrentLetter);
			UE_LOG(LogTemp, Warning, TEXT("Letter added to targetable : %s"), *ship->CurrentLetter.ToString());
		}
	}
	else
	{
		for(auto ship : CurrentTargets)
		{
			targetableLetters.AddUnique(ship->CurrentLetter);
		}
	}

	HighlightKeys(TranslateKeys(targetableLetters));

	TArray<FName> LettersToUntarget;
	for (auto Letter : HighlitedKeys)
	{
		if(!targetableLetters.Contains(Letter.GetFName()))
		{
			LettersToUntarget.Add(Letter.GetFName());
		}
	}

	UnhighlightKeys(TranslateKeys(LettersToUntarget));
	
}

void AKCD_Cube::ShipDestroyed(AKCD_Ship* Ship)
{
	Score += Ship->Reward * (ScoreMultiplicator * ( 1 + ComboCounter));
	RemoveTarget(Ship);
	
	if(!CurrentTargets.IsEmpty())
	{
		ComboCounter++;
	}

	OnScoreUpdateDelegate.Broadcast();
}

void AKCD_Cube::HighlightKeys(TArray<FKey> keysToHighlight)
{
	for (auto key : keysToHighlight)
	{
		if(key.GetFName() == "")
		{
			UE_LOG(LogTemp, Warning, TEXT("Invalid key to highlight"));
			continue;
		}
		Keys[key]->HighlightKey();

		HighlitedKeys.AddUnique(key);
		
		UE_LOG(LogTemp, Warning, TEXT("highlighted Key : %s"), *key.GetFName().ToString());
	}
}

void AKCD_Cube::UnhighlightKeys(TArray<FKey> KeysToUnHighlight)
{
	for(auto key : KeysToUnHighlight)
	{
		if(key.GetFName() == "")
		{
			UE_LOG(LogTemp, Warning, TEXT("Invalid key to release"));
			continue;
		}
		Keys[key]->KeyReleased_Keys();

		HighlitedKeys.Remove(key);
		
		UE_LOG(LogTemp, Warning, TEXT("Released Key : %s"), *key.GetFName().ToString());
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

void AKCD_Cube::NewTargets(const TArray<AKCD_Ship*>& ShipList)
{
	for (auto NewShip : ShipList)
	{
		if(!CurrentTargets.Contains(NewShip))
		{
			NewShip->OnShipDestroyedDelegate.AddDynamic(this, &AKCD_Cube::ShipDestroyed);
			CurrentTargets.Add(NewShip);
		}
	}
}

TArray<AKCD_Ship*> AKCD_Cube::GetCurrentTargets()
{
	return CurrentTargets;
}

