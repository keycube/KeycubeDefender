// Fill out your copyright notice in the Description page of Project Settings.


#include "KCD_Spawner.h"

#include "KCD_WordDictionnary.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AKCD_Spawner::AKCD_Spawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AKCD_Spawner::BeginPlay()
{
	Super::BeginPlay();

	//TODO : Remove this
	//Temporary way to spawn the ship using a looping timer
	FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this, &AKCD_Spawner::SpawnShip, 1);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle,  TimerDelegate, 2.0f, true);
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
	//We loop as long as we don't have an available word
	FKCD_Words ShipWord;
	while(!wordFound)
	{
		int randomInt = FMath::RandRange(0, possibleWords[0].WordList.Num() - 1);
		ShipWord = possibleWords[0].WordList[randomInt];
		wordFound = true;
	}
	
	//TODO : Set the position of the lane
	//Use a deferred spawn so we can set the ship's word before spawning it
	AKCD_Ship* Ship;
	Ship = GetWorld()->SpawnActorDeferred<AKCD_Ship>(Ships[ShipTier], this->GetTransform());
	
	Ship->SetWord(ShipWord);
	
	UGameplayStatics::FinishSpawningActor(Ship, this->GetTransform());

	ShipsAlive.Add(Ship);
}

void AKCD_Spawner::NextWave()
{
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

