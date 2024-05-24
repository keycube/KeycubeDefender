 // Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Delegates/DelegateCombinations.h"
#include "GameFramework/PlayerController.h"
#include "KCD_PlayerController.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnKeyPressDelegate, FKey, keyParam);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnKeyReleaseDelegate, FKey, keyParam);
UCLASS()
class KEYCUBEDEFENDER_API AKCD_PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category="Key press delegate")
	FOnKeyPressDelegate KeyPressDelegate;
	UPROPERTY(BlueprintAssignable, Category="Key release delegate")
	FOnKeyReleaseDelegate KeyReleaseDelegate;

protected:
	UFUNCTION(BlueprintCallable)
	void HandleKeyPressedDelegate(FKey key);

	UFUNCTION(BlueprintCallable)
	void HandleKeyReleasedDelegate(FKey key);
};
