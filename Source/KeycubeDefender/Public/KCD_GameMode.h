// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KCD_Spawner.h"
#include "GameFramework/GameModeBase.h"
#include "KCD_GameMode.generated.h"

/**
 * 
 */
UCLASS()
class KEYCUBEDEFENDER_API AKCD_GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	//Reference to the ship spawner
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Spawner)
	AKCD_Spawner* ShipSpawner;
	//Returns the ship spawner
	UFUNCTION(BlueprintCallable)
	AKCD_Spawner* GetShipSpawner();
};
