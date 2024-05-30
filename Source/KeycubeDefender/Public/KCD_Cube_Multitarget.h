// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KCD_Cube.h"
#include "KCD_Spawner.h"
#include "KCD_Cube_Multitarget.generated.h"

/**
 * 
 */
UCLASS()
class KEYCUBEDEFENDER_API AKCD_Cube_Multitarget : public AKCD_Cube
{
	GENERATED_BODY()

public:
	
private:
	UPROPERTY()
	TArray<AKCD_Ship*> CurrentTargets;
	
	virtual void KeyPress(FKey key) override;

	UFUNCTION()
	void NewTargets(const TArray<AKCD_Ship*>& ShipList);

	UFUNCTION()
	void RemoveTarget(AKCD_Ship* Ship);

};
