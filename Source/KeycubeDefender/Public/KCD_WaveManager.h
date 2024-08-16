// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KCD_GameMode.h"
#include "KCD_Ship.h"
#include "KCD_TypingStats.h"
#include "GameFramework/Actor.h"
#include "KCD_WaveManager.generated.h"

class AKCD_Lane;

USTRUCT()
struct FWave
{
	GENERATED_BODY()
	
	TMap<int, int> NumShipTier;

	float SpeedModifier;
	float SpawnTime;
	TArray<int> availableTiers;
};

USTRUCT()
struct FEncapsule
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	TArray<int> index;

	int operator[] (int32 i)
	{
		return index[i];
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWaveComplete, int, CurrentWave);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnVictory);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShipSpawn);
UCLASS()
class KEYCUBEDEFENDER_API AKCD_WaveManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AKCD_WaveManager();

	//List to keep track of the ships alive. Also useful to know when a wave is over
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Variables")
	TArray<AKCD_Ship*> ShipsAlive;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when the game is over
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	//Ships BP used as base to instantiate new ones
	UPROPERTY(EditDefaultsOnly)
	TMap<int, TSubclassOf<AKCD_Ship>> Ships;

	//Data table containing the available words
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Variables")
	UDataTable* WordBank;

	UPROPERTY(EditAnywhere, Category="Variables")
	TArray<FEncapsule> WordIndexUsed;

	//Spawns the ship of the specified tier and sets it's letter
	UFUNCTION(BlueprintCallable)
	void PrepareShip(int ShipTier);

	//Go onto the next wave
	UFUNCTION(BlueprintCallable)
	void NextWave();

	//Spawns ships according to the data in the WaveData
	UFUNCTION(BlueprintCallable)
	void PlayWaveSequence();

	//Generates data for the requested wave. Data is determined by
	//values from LikelihoodValues and base data + dampener for speed and spawn
	//time
	UFUNCTION(BlueprintCallable)
	void GenerateWaveData(int waveIndex);

private:
	FTimerHandle SpawnTimerHandle;
	FTimerHandle NewWaveTimerHandle;

	//Removes a ship from the list of living ones and frees it's word
	UFUNCTION()
	void RemoveShip(AKCD_Ship* Ship);

	//Returns a random lane
	UFUNCTION()
	AKCD_Lane* FetchRandomLane();

	//Called when the game is finished
	UFUNCTION()
	void GameFinished();

	//Makes an average of the stats of the game.
	UFUNCTION()
	void AverageStats();

	//Writes the specified stats to a file (only main targets)
	UFUNCTION()
	void WriteStats(FKCD_TypingStats MainStat);
	//Writes the specified stats to a file (only main targets)
	void WriteStats(FKCD_TypingStats MainStat, FKCD_TypingStats AltStat);
	
	UPROPERTY()
	AKCD_GameMode* GameModeInstance;

	UPROPERTY(EditAnywhere)
	int CurrentWaveIndex = 0;
	
	UPROPERTY()
	FWave CurrentWaveData;

	UPROPERTY(EditAnywhere)
	TMap<int, float> LikelihoodValues = {{0,1},
		{1, 0.4}, {2, 0.2}};

	UPROPERTY(EditAnywhere)
	float BaseSpeed = 0.8;
	UPROPERTY(EditAnywhere)
	float SpeedDampener = 25;
	UPROPERTY(EditAnywhere)
	float BaseSpawnTime = 3;
	UPROPERTY(EditAnywhere)
	float SpawnDampener = 0.05;

	UPROPERTY()
	AKCD_LaneHolder* LaneHolder;

	UPROPERTY()
	TArray<FKCD_TypingStats> MainTypingStats;
	UPROPERTY()
	TArray<FKCD_TypingStats> AltTypingStats;
	
public:	

	UPROPERTY(BlueprintAssignable)
	FOnVictory OnVictoryDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnWaveComplete OnWaveCompleteDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnShipSpawn OnShipSpawnDelegate;

	//Returns all the ships who are available (not already in the destroy state)
	//who's next letter to be hit is the one provided
	UFUNCTION(BlueprintCallable)
	TArray<AKCD_Ship*> GetValidShips(FName Letter);
	
	UFUNCTION(BlueprintCallable)
	void AddStats(FKCD_TypingStats StatsToAdd);
	
};
