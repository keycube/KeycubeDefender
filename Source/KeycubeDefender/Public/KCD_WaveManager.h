// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KCD_GameMode.h"
#include "KCD_Ship.h"
#include "KCD_WaveData.h"
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

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	//Ships BP used as base to instantiate new ones
	UPROPERTY(EditDefaultsOnly)
	TMap<int, TSubclassOf<AKCD_Ship>> Ships;

	//Data table containing the available words
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Variables")
	UDataTable* WordBank;

	TArray<FEncapsule> WordIndexUsed;
	
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
	void ReadCurrentWaveData(int waveIndex);

private:
	FTimerHandle SpawnTimerHandle;
	FTimerHandle NewWaveTimerHandle;

	UFUNCTION()
	void RemoveShip(AKCD_Ship* Ship);

	UFUNCTION()
	FVector LaneTransform(AKCD_Lane* Lane);

	UFUNCTION()
	AKCD_Lane* FetchRandomLane();

	UFUNCTION()
	void ShipCrashed();
	
	UPROPERTY()
	AKCD_GameMode* GameModeInstance;

	UPROPERTY()
	int CurrentWaveIndex = 0;
	UPROPERTY()
	FWave CurrentWaveData;

	UPROPERTY()
	AKCD_LaneHolder* LaneHolder;

	
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintAssignable)
	FOnVictory OnVictoryDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnWaveComplete OnWaveCompleteDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnShipSpawn OnShipSpawnDelegate;

	//Returns the closest ship available (not already in the destroy state)
	//who's next letter to be hit is the one provided
	UFUNCTION(BlueprintCallable)
	TArray<AKCD_Ship*> GetValidShips(FName Letter);

	
};
