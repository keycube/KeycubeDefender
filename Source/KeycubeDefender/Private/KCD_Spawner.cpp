// Fill out your copyright notice in the Description page of Project Settings.


#include "KCD_Spawner.h"
#include "KCD_LaneHolder.h"
#include "KCD_WordDictionnary.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetStringLibrary.h"

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
	
	if(WaveData == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("WAVE DATA NOT FOUND"));
		exit(666);
	}

	//Get the current game mode and cast it to the required game mode
	GameModeInstance = Cast<AKCD_GameMode>(UGameplayStatics::GetGameMode(this));

	if(!GameModeInstance->IsValidLowLevel())
	{
		UE_LOG(LogTemp, Warning, TEXT("Game mode is invalid"));
	}
	
}

void AKCD_Spawner::SpawnShip(int ShipTier)
{
	//Get all the possible words for the ship's tier
	FString ShipTierString = FString::FromInt(ShipTier);
	FKCD_WordDictionnary* possibleWords = WordBank->FindRow<FKCD_WordDictionnary>(FName(ShipTierString), "");

	bool wordFound = false;

	//TODO : Check if the word is available and make a break condition so we don't have an infinite loop
	//Get a random word in the list of possible ones
	//We loop so we can find an available word
	int ShipWordIndex = -1;
	for(int x = 0; x <= 20; x++)
	{
		ShipWordIndex = FMath::RandRange(0, possibleWords[0].WordList.Num() - 1);
		if(WordIndexUsed[ShipTier].index.Contains(ShipWordIndex))
			continue;
		
		wordFound = true;
		WordIndexUsed[ShipTier].index.Add(ShipWordIndex);
		break;
	}

	if(!wordFound)
	{
		UE_LOG(LogTemp, Warning, TEXT("Word not found"));
		return;
	}
	
	//Use a deferred spawn so we can set the ship's word before spawning it
	AKCD_Ship* Ship;

	auto lane = FetchRandomLane();
	if(lane == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Couldn't get a valid lane"));
	}

	FTransform spawnTransform{
		this->GetTransform().GetRotation(),                 // Rotation
		LaneTransform(lane),  // Translation
		FVector{1.0f, 1.0f, 1.0f}   // Scale
	};
	
	Ship = GetWorld()->SpawnActorDeferred<AKCD_Ship>(Ships[ShipTier], spawnTransform);

	//Setting the ship's variables
	Ship->Initialize(ShipTier, possibleWords[0].WordList[ShipWordIndex], ShipWordIndex, 1.0);
	
	UGameplayStatics::FinishSpawningActor(Ship, spawnTransform);

	ShipsAlive.Add(Ship);

	Ship->OnShipDestroyedDelegate.AddDynamic(this, &AKCD_Spawner::RemoveShip);
}

void AKCD_Spawner::NextWave()
{
	CurrentWaveIndex++;

	if(CurrentWaveIndex > WaveData->GetRowNames().Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("No more waves"));
	}

	FKCD_WaveData* CurrentWave = WaveData->FindRow<FKCD_WaveData>(FName(FString::FromInt(CurrentWaveIndex)), "");

	ReadCurrentWaveData(*CurrentWave);
	
	PlayWaveSequence();
}

void AKCD_Spawner::PlayWaveSequence()
{
	int numbOfShips = CurrentWaveData.NumShipTier[0] + CurrentWaveData.NumShipTier[1] + CurrentWaveData.NumShipTier[2] ;
	TArray availableTiers = {0, 1, 2};

	if(CurrentWaveData.NumShipTier[0] == 0)
		availableTiers.Remove(0);
	if(CurrentWaveData.NumShipTier[1] == 0)
		availableTiers.Remove(1);
	if(CurrentWaveData.NumShipTier[2] == 0)
		availableTiers.Remove(2);

	for(int i = numbOfShips; i>0; i--)
	{
		int ShipTier = FMath::RandRange(0, availableTiers.Num() - 1);

		SpawnShip(ShipTier);

		--CurrentWaveData.NumShipTier[ShipTier];
		if(CurrentWaveData.NumShipTier[ShipTier] == 0)
			availableTiers.Remove(ShipTier);
		
	}
	
}

void AKCD_Spawner::ReadCurrentWaveData(FKCD_WaveData& Wave)
{
	if(Wave.NumberOfT0 <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Wave is invalid"));
		return;
	}
	CurrentWaveData.NumShipTier[0] = Wave.NumberOfT0;
	CurrentWaveData.NumShipTier[1] = Wave.NumberOfT1;
	CurrentWaveData.NumShipTier[2] = Wave.NumberOfT2;
	CurrentWaveData.SpawnTime = Wave.SpawnSpeed;
	CurrentWaveData.SpeedModifier = Wave.SpeedModifier;
}

void AKCD_Spawner::RemoveShip(AKCD_Ship* Ship)
{
	ShipsAlive.Remove(Ship);
	Ship->OnShipDestroyedDelegate.RemoveAll(this);
	WordIndexUsed[Ship->Tier].index.Remove(Ship->WordIndex);
}

FVector AKCD_Spawner::LaneTransform(AKCD_Lane* Lane)
{
	return Lane->GetTransform().GetLocation();
}

AKCD_Lane* AKCD_Spawner::FetchRandomLane()
{
	AKCD_LaneHolder* LaneHolder = GameModeInstance->GetLaneHolder();
	return LaneHolder->Lanes[FMath::RandRange(0, LaneHolder->Lanes.Num() - 1)];
}

// Called every frame
void AKCD_Spawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

AKCD_Ship* AKCD_Spawner::GetClosestShip(FName letter)
{
	//Verify if there is a ship alive
	if(ShipsAlive.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("Ship list is empty"));
		return nullptr;
	}

	//We go trough the list of ship to find one who's current
	//letter to destroy is the one we try to shoot
	for(AKCD_Ship* shipChecked : ShipsAlive)
	{
		if(shipChecked->isDestroyed)
			continue;
		if(shipChecked->LettersInstances[0]->CurrentLetter == letter)
		{
			return shipChecked;
		}
	}

	//No ship is found
	return nullptr;
}

