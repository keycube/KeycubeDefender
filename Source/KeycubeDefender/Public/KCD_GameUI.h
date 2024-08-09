// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextRenderComponent.h"
#include "KCD_GameUI.generated.h"

/**
 * 
 */
UCLASS()
class KEYCUBEDEFENDER_API UKCD_GameUI : public UUserWidget
{
	GENERATED_BODY()

	//Writes the generated tag to the stats files
	UFUNCTION(BlueprintCallable)
	void WriteNewUser();

	UFUNCTION()
	FString GenerateUser();

	inline static const char alphanum[] =
"0123456789"
"!@#$%^&*"
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz";

	int stringLength = sizeof(alphanum) - 1;

	char genRandom() const
	{
		return alphanum[rand() % stringLength];
	}
};
