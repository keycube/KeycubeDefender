// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "KCD_GameMode.generated.h"

class AKCD_WaveManager;
class AKCD_LaneHolder;

/**
 * 
 */
UCLASS()
class KEYCUBEDEFENDER_API AKCD_GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Spawner)
	AKCD_WaveManager* ShipSpawner;

	UFUNCTION(BlueprintCallable)
	AKCD_WaveManager* GetShipSpawner();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Spawner)
	AKCD_LaneHolder* LaneHolder;

	UFUNCTION(BlueprintCallable)
	AKCD_LaneHolder* GetLaneHolder();
};
