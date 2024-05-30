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
	//Target who will receive our current input
	UPROPERTY(EditAnywhere, Category=Variable)
	AKCD_Ship* CurrentTarget;

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

	//Removes the current target and unsubscribes from the delegate
	UFUNCTION(BlueprintCallable)
	void LooseCurrentTarget(AKCD_Ship* ship);

	UFUNCTION(BlueprintCallable)
	void ShipDestroyed(AKCD_Ship* ship);

	//Mark the letters who would result in a good hit
	UFUNCTION(BlueprintCallable)
	void HighlightKeys(TArray<FKey> keysToHighlight);

	//Reference to the GameMode
	UPROPERTY()
	AKCD_GameMode* GameModeInstance;

	UPROPERTY()
	AKCD_Spawner* SpawnerInstance;
	
	//Advance or reset the score multiplicator 
	UFUNCTION()
	void UpdateMultiplicator(bool Success);

	

public:	
	//Set a new target
	UFUNCTION(BlueprintCallable)
	bool NewTarget(AKCD_Ship* ship);

	//Returns the current target
	UFUNCTION(BlueprintCallable)
	AKCD_Ship* GetCurrentTarget();

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

};
