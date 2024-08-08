// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "KVA_SaveCubeOption.generated.h"

/**
 * 
 */
UCLASS()
class KVA_API UKVA_SaveCubeOption : public USaveGame
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ShowCube = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool InvertRotation = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ShouldRotate = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ShowNextHighligh = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector NextHighlighColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ShowTypeHighligh = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector TypeHighlighColor;
	
	static const FString SlotName;

	UFUNCTION(BlueprintCallable)
	FString GetSlotName() {return SlotName;}
};
