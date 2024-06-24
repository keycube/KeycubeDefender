// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KCD_GameMode.h"
#include "KCD_Keys.h"
#include "KCD_Ship.h"
#include "GameFramework/Actor.h"
#include "KCD_Cube.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnScoreUpdate);
UCLASS()
class KEYCUBEDEFENDER_API AKCD_Cube : public APawn
{
	GENERATED_BODY()

	//component of the cube object
	UPROPERTY(VisibleAnywhere, Category=Mesh)
	UStaticMeshComponent* CubeMesh;
	
public:	
	// Sets default values for this actor's properties
	AKCD_Cube();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Map of association between the inputs and the
	//keys on the cube
	UPROPERTY(VisibleAnywhere, Category=Keys)
	TMap<FKey, AKCD_Keys*> Keys;

	//Map of association between the inputs and the
	//keys on the cube
	UPROPERTY(VisibleAnywhere, Category=Keys)
	TArray<FKey> HighlitedKeys;
	
	//Target who will receive our current input
	UPROPERTY(EditAnywhere, Category=Variable)
	TArray<AKCD_Ship*> CurrentTargets;
	//Target who will not be removed if the wrong input in entered
	UPROPERTY(EditAnywhere, Category=Variable)
	AKCD_Ship* PrimaryTarget;

	//Reference to the GameMode
	UPROPERTY()
	AKCD_GameMode* GameModeInstance;
	//Reference to the spawner
	UPROPERTY()
	AKCD_Spawner* SpawnerInstance;

	UFUNCTION(BlueprintCallable)
	void ShipDestroyed(AKCD_Ship* ship);

	//Mark the letters who would result in a good hit
	UFUNCTION(BlueprintCallable)
	void HighlightKeys(TArray<FKey> keysToHighlight);

	//Mark the letters who would result in a good hit
	UFUNCTION(BlueprintCallable)
	void UnhighlightKeys(TArray<FKey> keysToHighlight);
	
	//Advance or reset the score multiplicator 
	UFUNCTION()
	void UpdateMultiplicator(bool Success);

private:
	//Fill the map of association between the inputs and the
	//keys on the cube
	UFUNCTION()
	void FillKeyMap();

	//Function called when a key is pressed
	//Gives visual feedback of the key pressed, and tries to hit a target
	UFUNCTION(BlueprintCallable)
	void KeyPress(FKey key);

	//Function called when a key is released
	//removes the visual feedback of the key pressed
	UFUNCTION(BlueprintCallable)
	void KeyRelease(FKey key);

	//Add the new possible targets
	UFUNCTION(BlueprintCallable)
	void NewTargets(const TArray<AKCD_Ship*>& ShipList);
	//Removes the specified ship from the targets
	UFUNCTION()
	void RemoveTarget(AKCD_Ship* Ship);
	//Takes the FName of the letters and translates them to FKeys
	UFUNCTION()
	TArray<FKey> TranslateKeys(TArray<FName> KeysToTranslate);
	//Removes the specified ship from the targets
	UFUNCTION()
	void UpdateHighlight();

	UFUNCTION()
	void FindPrimaryTarget();
	
	UPROPERTY(EditAnywhere, Category="Sound")
	USoundBase* TypeSound;

	

public:	

	//Returns the current target
	UFUNCTION(BlueprintCallable)
	TArray<AKCD_Ship*> GetCurrentTargets();

	//Event delegate for ship destruction
	UPROPERTY(BlueprintAssignable)
	FOnScoreUpdate OnScoreUpdateDelegate;

	//Score variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Variables")
	int Score = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Variables")
	int MultiplicatorGauge = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Variables")
	int ScoreMultiplicator = 1;

	//Numbers of ships destroy without having an empty target list
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Variables")
	int ComboCounter = 0;

};
