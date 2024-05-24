// Fill out your copyright notice in the Description page of Project Settings.


#include "KCD_Spawner.h"

#include "KCD_WordDictionnary.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AKCD_Spawner::AKCD_Spawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AKCD_Spawner::BeginPlay()
{
	Super::BeginPlay();

	FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this, &AKCD_Spawner::SpawnShip, 1);
	
	GetWorld()->GetTimerManager().SetTimer(TimerHandle,  TimerDelegate, 2.0f, true);
}

void AKCD_Spawner::SpawnShip(int ShipTier)
{
	//TODO : THIS IS A TEST, REMOVE IT LATER
	ShipTier = FMath::RandRange(0, 2);
	
	FString ShipTierString = FString::FromInt(ShipTier);
	FKCD_WordDictionnary* possibleWords = WordBank->FindRow<FKCD_WordDictionnary>(FName(ShipTierString), "");

	bool wordFound = false;

	FKCD_Words ShipWord;
	while(!wordFound)
	{
		int randomInt = FMath::RandRange(0, possibleWords[0].WordList.Num() - 1);
		ShipWord = possibleWords[0].WordList[randomInt];
		wordFound = true;
	}
	
	//TODO : Set the position of the lane
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
	if(ShipsAlive.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("Ship list is empty"));
		return nullptr;
	}
	
	for(AKCD_Ship* shipChecked : ShipsAlive)
	{
		if(shipChecked->LettersInstances.Num() <= 0)
			continue;
		if(shipChecked->LettersInstances[0]->CurrentLetter == letter)
		{
			return shipChecked;
		}
	}

	return nullptr;
}

