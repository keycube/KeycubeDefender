// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KCD_GameMode.h"
#include "KCD_Lane.h"
#include "KCD_LaneHolder.h"
#include "KCD_Ship.h"
#include "KCD_WaveData.h"
#include "KCD_WordDictionnary.h"
#include "Subsystems/WorldSubsystem.h"
#include "KCD_SpawnerWorldSubsystem.generated.h"

USTRUCT()
struct FWaveToo
{
	GENERATED_BODY()
	
	TMap<int, int> NumShipTier;

	float SpeedModifier;
	float SpawnTime;
};

USTRUCT()
struct FEncapsuleToo
{
	GENERATED_BODY()
	TArray<int> index;

	int operator[] (int32 i)
	{
		return index[i];
	}
};

/**
 * 
 */
UCLASS()
class KEYCUBEDEFENDER_API UKCD_SpawnerWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:	

	//List to keep track of the ships alive. Also useful to know when a wave is over
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Variables")
	TArray<AKCD_Ship*> ShipsAlive;

protected:
	// Called when the game starts or when spawned
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	//Ships BP used as base to instantiate new ones
	UPROPERTY(EditDefaultsOnly)
	TMap<int, TSubclassOf<AKCD_Ship>> Ships;

	//Data table containing the available words
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Variables")
	UDataTable* WordBank;

	TArray<FEncapsuleToo> WordIndexUsed;
	
	// //TODO : MAKE WAVE DATA
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Variables")
	UDataTable* WaveData;

	//Spawns the ship of the specified tier and sets it's letter
	UFUNCTION(BlueprintCallable)
	void SpawnShip(int ShipTier);

	//Go onto the next wave
	UFUNCTION(BlueprintCallable)
	void NextWave();

	// //Spawns ships according to the data in the WaveData
	UFUNCTION(BlueprintCallable)
	void PlayWaveSequence();

	// Reads the data table and fills an FWave 
	// struct with the data of the current wave
	UFUNCTION(BlueprintCallable)
	void ReadCurrentWaveData(FKCD_WaveData& Wave);

private:
	FTimerHandle TimerHandle;

	UFUNCTION()
	void RemoveShip(AKCD_Ship* Ship);

	UFUNCTION()
	FVector LaneTransform(AKCD_Lane* Lane);

	UFUNCTION()
	AKCD_Lane* FetchRandomLane();

	UPROPERTY()
	AKCD_GameMode* GameModeInstance;

	UPROPERTY()
	int CurrentWaveIndex = 0;
	UPROPERTY()
	FWaveToo CurrentWaveData;
	
public:
	
	//Returns the closest ship available (not already in the destroy state)
	//who's next letter to be hit is the one provided
	UFUNCTION(BlueprintCallable)
	AKCD_Ship* GetClosestShip(FName letter);

};
