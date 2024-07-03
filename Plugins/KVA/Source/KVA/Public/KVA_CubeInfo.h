// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "KVA_CubeInfo.generated.h"

/**
 * 
 */

USTRUCT()
struct FKeyRowLoad
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<FKey> Keys;

	FKey operator[] (int32 i)
	{
		return Keys[i];
	}
};

USTRUCT()
struct FKeyFaceLoad
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<FKeyRowLoad> Face;

	FKeyRowLoad operator[] (int32 i)
	{
		return Face[i];
	}
};

UCLASS()
class KVA_API UKVA_CubeInfo : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TArray<FKeyFaceLoad> SavedKeyMatrix;

	static const FString SlotName;
};
