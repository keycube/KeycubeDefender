// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KVA_CubeVisual.h"
#include "GameFramework/SaveGame.h"
#include "KVA_Cube_SaveType.generated.h"

/**
 * 
 */
UCLASS()
class KVA_API UKVA_Cube_SaveType : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CubeType)
	TSubclassOf<AKVA_CubeVisual> CubeType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CubeType)
	FString Name;

	static const FString SlotName;

	UFUNCTION(BlueprintCallable, Category = CubeType)
	FString GetSlotName() {return SlotName;}
};
