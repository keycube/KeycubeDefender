// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KCD_Ship.h"
#include "GameFramework/Actor.h"
#include "KCD_Spawner.generated.h"

UCLASS()
class KEYCUBEDEFENDER_API AKCD_Spawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AKCD_Spawner();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Variables")
	TArray<AKCD_Ship*> ShipsAlive;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly)
	TMap<int, TSubclassOf<AKCD_Ship>> Ships;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Variables")
	UDataTable* WordBank;
	
	// //TODO : MAKE WAVE DATA
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Variables")
	// KCD_WaveData CurrentWaveData;
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Variables")
	// TArray<KCD_WaveData> Waves;

	UFUNCTION(BlueprintCallable)
	void SpawnShip(int ShipTier);

	UFUNCTION(BlueprintCallable)
	void NextWave();

	// UFUNCTION(BlueprintCallable)
	// void PlayWaveSequence(KCD_WaveData Wave);

private:
	FTimerHandle TimerHandle;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	AKCD_Ship* GetClosestShip(FName letter);

	
};
