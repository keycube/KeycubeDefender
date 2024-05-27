// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KCD_Ship.h"
#include "GameFramework/Actor.h"
#include "KCD_Spawner.generated.h"

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

UCLASS()
class KEYCUBEDEFENDER_API AKCD_Spawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AKCD_Spawner();

	//List to keep track of the ships alive. Also useful to know when a wave is over
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Variables")
	TArray<AKCD_Ship*> ShipsAlive;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Ships BP used as base to instantiate new ones
	UPROPERTY(EditDefaultsOnly)
	TMap<int, TSubclassOf<AKCD_Ship>> Ships;

	//Data table containing the available words
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Variables")
	UDataTable* WordBank;

	TArray<FEncapsule> WordIndexUsed;
	
	// //TODO : MAKE WAVE DATA
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Variables")
	// KCD_WaveData CurrentWaveData;
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Variables")
	// TArray<KCD_WaveData> Waves;

	//Spawns the ship of the specified tier and sets it's letter
	UFUNCTION(BlueprintCallable)
	void SpawnShip(int ShipTier);

	//Go onto the next wave
	UFUNCTION(BlueprintCallable)
	void NextWave();

	// //Spawns ships according to the data in the WaveData
	// UFUNCTION(BlueprintCallable)
	// void PlayWaveSequence(KCD_WaveData Wave);

private:
	FTimerHandle TimerHandle;

	UFUNCTION()
	void RemoveShip(AKCD_Ship* Ship);
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Returns the closest ship available (not already in the destroy state)
	//who's next letter to be hit is the one provided
	UFUNCTION(BlueprintCallable)
	AKCD_Ship* GetClosestShip(FName letter);

	
};
