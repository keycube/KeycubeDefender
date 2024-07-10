// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "KCD_Leaderboard.generated.h"

/**
 * 
 */
UCLASS()
class KEYCUBEDEFENDER_API UKCD_Leaderboard : public USaveGame
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FString Name;

	UPROPERTY(EditAnywhere)
	int Score;

	static const FString SlotName;
};
