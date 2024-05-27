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

	//TODO : Remove this
	//Temporary way to spawn the ship using a looping timer
	FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this, &AKCD_Spawner::SpawnShip, 1);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle,  TimerDelegate, 2.0f, true);

	//Get the current game mode and cast it to the required game mode
	GameModeInstance = Cast<AKCD_GameMode>(UGameplayStatics::GetGameMode(this));

	if(!GameModeInstance->IsValidLowLevel())
	{
		UE_LOG(LogTemp, Warning, TEXT("Game mode is invalid"));
	}
	
}

void AKCD_Spawner::SpawnShip(int ShipTier)
{
	//TODO : THIS IS A TEST, REMOVE IT LATER
	//Temporary way to simulate the spawn of different ship tiers
	ShipTier = FMath::RandRange(0, 2);

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
		UE_LOG(LogTemp, Warning, TEXT("ADDING Ship tier : %s"), *FString::FromInt(ShipTier));
		UE_LOG(LogTemp, Warning, TEXT("ADDING Ship word index : %s"), *FString::FromInt(ShipWordIndex));
		break;
	}

	if(!wordFound)
	{
		UE_LOG(LogTemp, Warning, TEXT("Word not found"));
		return;
	}
	
	//TODO : Set the position of the lane
	//Use a deferred spawn so we can set the ship's word before spawning it
	AKCD_Ship* Ship;
	AKCD_LaneHolder* LaneHolder = GameModeInstance->GetLaneHolder();

	if(LaneHolder != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Got the lane holder!"));
	}
	
	Ship = GetWorld()->SpawnActorDeferred<AKCD_Ship>(Ships[ShipTier], this->GetTransform());

	//Setting the ship's variables
	Ship->Tier = ShipTier;
	Ship->SetWord(possibleWords[0].WordList[ShipWordIndex]);
	Ship->WordIndex = ShipWordIndex;
	
	UGameplayStatics::FinishSpawningActor(Ship, this->GetTransform());

	ShipsAlive.Add(Ship);

	Ship->OnShipDestroyedDelegate.AddDynamic(this, &AKCD_Spawner::RemoveShip);
}

void AKCD_Spawner::NextWave()
{
}

void AKCD_Spawner::RemoveShip(AKCD_Ship* Ship)
{
	ShipsAlive.Remove(Ship);
	Ship->OnShipDestroyedDelegate.RemoveAll(this);
	WordIndexUsed[Ship->Tier].index.Remove(Ship->WordIndex);

	UE_LOG(LogTemp, Warning, TEXT("REMOVING Ship tier : %s"), *FString::FromInt(Ship->Tier));
	UE_LOG(LogTemp, Warning, TEXT("REMOVING Ship word index : %s"), *FString::FromInt(Ship->WordIndex));
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

