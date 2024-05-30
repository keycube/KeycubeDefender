// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KCD_Spawner.h"
#include "KCD_Spawner_MultiTarget.generated.h"

/**
 * 
 */
UCLASS()
class KEYCUBEDEFENDER_API AKCD_Spawner_MultiTarget : public AKCD_Spawner
{
	GENERATED_BODY()

public:
	UFUNCTION()
	TArray<AKCD_Ship*> GetClosestShips(FName Letter);
	
};
