// Fill out your copyright notice in the Description page of Project Settings.


#include "KCD_Spawner.h"

#include "KCD_LaneHolder.h"
#include "KCD_WordDictionnary.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AKCD_Spawner::AKCD_Spawner()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	WordIndexUsed.Add(FEncapsule{});
	WordIndexUsed.Add(FEncapsule{});
	WordIndexUsed.Add(FEncapsule{});
}

// Called when the game starts or when spawned
void AKCD_Spawner::BeginPlay()
{
	Super::BeginPlay();

	if (WaveData == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("WAVE DATA NOT FOUND"));
	}

	//Get the current game mode and cast it to the required game mode
	GameModeInstance = Cast<AKCD_GameMode>(UGameplayStatics::GetGameMode(this));

	FTimerHandle TimerHandleGamemodeRefs;

	// Fetch the references set in the gamemode after a delay
	//to allow it to finish fetching them
	GetWorld()->GetTimerManager().SetTimer(TimerHandleGamemodeRefs, [&]()
	{
		LaneHolder = GameModeInstance->GetLaneHolder();
		if(LaneHolder == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("LaneHolder is invalid"));
		} else
		{
			LaneHolder->OnShipCrashedDelegate.AddDynamic(this, &AKCD_Spawner::ShipCrashed);
		}
	},  0.1, false);

	NextWave();
}

void AKCD_Spawner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	UWorld* w= GetWorld() ;

	if(w->IsValidLowLevel())
	{
		w->GetTimerManager().ClearAllTimersForObject(this);
	}
}

void AKCD_Spawner::SpawnShip(int ShipTier)
{
	//Get all the possible words for the ship's tier
	FString ShipTierString = FString::FromInt(ShipTier);
	FKCD_WordDictionnary* possibleWords = WordBank->FindRow<FKCD_WordDictionnary>(FName(ShipTierString), "");

	bool wordFound = false;

	//Get a random word in the list of possible ones
	//We loop so we can find an available word
	int ShipWordIndex = -1;
	for (int x = 0; x <= 20; x++)
	{
		ShipWordIndex = FMath::RandRange(0, possibleWords[0].WordList.Num() - 1);
		if (WordIndexUsed[ShipTier].index.Contains(ShipWordIndex))
			continue;

		wordFound = true;
		WordIndexUsed[ShipTier].index.Add(ShipWordIndex);
		break;
	}

	if (!wordFound)
	{
		UE_LOG(LogTemp, Warning, TEXT("Word not found"));
		return;
	}

	//Use a deferred spawn so we can set the ship's word before spawning it
	AKCD_Ship* Ship;

	auto lane = FetchRandomLane();
	if (lane == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Couldn't get a valid lane"));
		return;
	}

	Ship = lane->SpawnShip(Ships[ShipTier], ShipWordIndex, possibleWords[0].WordList[ShipWordIndex], CurrentWaveData.SpeedModifier);

	ShipsAlive.Add(Ship);

	OnShipSpawnDelegate.Broadcast();

	Ship->OnShipDestroyedDelegate.AddDynamic(this, &AKCD_Spawner::RemoveShip);
}

void AKCD_Spawner::NextWave()
{
	CurrentWaveIndex++;

	if (CurrentWaveIndex > WaveData->GetRowNames().Num())
	{
		OnVictoryDelegate.Broadcast();
		return;
	}
	
	ReadCurrentWaveData(CurrentWaveIndex);

	//Start a looping timer to spawn ships at an interval
	GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &AKCD_Spawner::PlayWaveSequence,
		CurrentWaveData.SpawnTime, true);
}

void AKCD_Spawner::PlayWaveSequence()
{
	int ShipTier = CurrentWaveData.availableTiers[FMath::RandRange(0, CurrentWaveData.availableTiers.Num() - 1)];

	SpawnShip(ShipTier);

	--CurrentWaveData.NumShipTier[ShipTier];
	//Once all the ships for a tier are spawned,
	//remove the tier from the list of possible tiers
	if (CurrentWaveData.NumShipTier[ShipTier] <= 0)
		CurrentWaveData.availableTiers.Remove(ShipTier);

	//Stop the spawning loop when all ships are spawned
	if(CurrentWaveData.availableTiers.IsEmpty())
	{
		GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle);
		CurrentWaveData.NumShipTier.Empty();
	}
}

void AKCD_Spawner::ReadCurrentWaveData(int WaveIndex)
{
	const FKCD_WaveData* CurrentWave = WaveData->FindRow<FKCD_WaveData>(FName(FString::FromInt(WaveIndex)), "");

	//Check if the wave is valid (there should always be at least one ship of T0)
	if (CurrentWave->NumberOfT0 <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Wave is invalid"));
		return;
	}
	//Fill the current wave number of ships
	CurrentWaveData.NumShipTier.Add(0, CurrentWave->NumberOfT0);
	if(CurrentWaveData.NumShipTier[0] > 0)
		CurrentWaveData.availableTiers.Add(0);
	
	CurrentWaveData.NumShipTier.Add(1, CurrentWave->NumberOfT1);
	if(CurrentWaveData.NumShipTier[1] > 0)
		CurrentWaveData.availableTiers.Add(1);
	
	CurrentWaveData.NumShipTier.Add(2, CurrentWave->NumberOfT2);
	if(CurrentWaveData.NumShipTier[2] > 0)
		CurrentWaveData.availableTiers.Add(2);

	//fill the variable for the speed of this wave
	CurrentWaveData.SpawnTime = CurrentWave->SpawnSpeed;
	CurrentWaveData.SpeedModifier = CurrentWave->SpeedModifier;
}

void AKCD_Spawner::RemoveShip(AKCD_Ship* Ship)
{
	ShipsAlive.Remove(Ship);
	Ship->OnShipDestroyedDelegate.RemoveAll(this);
	WordIndexUsed[Ship->Tier].index.Remove(Ship->WordIndex);

	//When it is the last ship of the wave, we wait a bit then start the new wave
	if(ShipsAlive.IsEmpty() && CurrentWaveData.availableTiers.IsEmpty())
	{
		if(CurrentWaveIndex != WaveData->GetRowNames().Num())
			OnWaveCompleteDelegate.Broadcast(CurrentWaveIndex);
		
		GetWorld()->GetTimerManager().SetTimer(NewWaveTimerHandle, this, &AKCD_Spawner::NextWave,
			3, false);
	}
}

FVector AKCD_Spawner::LaneTransform(AKCD_Lane* Lane)
{
	return Lane->GetTransform().GetLocation();
}

AKCD_Lane* AKCD_Spawner::FetchRandomLane()
{
	return LaneHolder->Lanes[FMath::RandRange(0, LaneHolder->Lanes.Num() - 1)];
}

void AKCD_Spawner::ShipCrashed()
{
	UE_LOG(LogTemp, Warning, TEXT("REEEEEE"));
}

// Called every frame
void AKCD_Spawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

TArray<AKCD_Ship*> AKCD_Spawner::GetValidShips(FName Letter)
{
	//Verify if there is a ship alive
	if (ShipsAlive.IsEmpty())
		return TArray<AKCD_Ship*>();

	TArray<AKCD_Ship*> matchingShips;

	//We go trough the list of ship to find one who's first
	//letter to destroy is the one we try to shoot
	for (AKCD_Ship* shipChecked : ShipsAlive)
	{
		if (shipChecked->isDestroyed)
			continue;
		if (shipChecked->LettersInstances[0]->CurrentLetter == Letter)
		{
			matchingShips.Add(shipChecked);
		}
	}

	return matchingShips;
	
}
