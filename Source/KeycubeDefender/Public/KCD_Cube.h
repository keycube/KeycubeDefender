// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KCD_GameMode.h"
#include "KCD_PlayerScore.h"
#include "KCD_Ship.h"
#include "KVA_CubeVisual.h"
#include "KCD_Cube.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnScoreUpdate);
UCLASS()
class KEYCUBEDEFENDER_API AKCD_Cube : public APawn
{
	GENERATED_BODY()

	//component of the cube object
	UPROPERTY(VisibleAnywhere, Category=Mesh)
	USceneComponent* RootObject;
	
public:	
	// Sets default values for this actor's properties
	AKCD_Cube();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	//Target who will receive our current input
	UPROPERTY(EditAnywhere, Category=Variable)
	TArray<AKCD_Ship*> CurrentTargets;
	//Target who will not be removed if the wrong input in entered
	UPROPERTY(EditAnywhere, Category=Variable)
	AKCD_Ship* PrimaryTarget;
	//Keycube Visual aid
	UPROPERTY(EditAnywhere, Category = Variable)
	AKVA_CubeVisual* Cube;

	//Reference to the GameMode
	UPROPERTY()
	AKCD_GameMode* GameModeInstance;
	//Reference to the spawner
	UPROPERTY()
	AKCD_WaveManager* SpawnerInstance;

	//Effect of the destruction of the ship for the cube
	UFUNCTION(BlueprintCallable)
	void ShipDestroyed(AKCD_Ship* ship);
	
	//Advance or reset the score multiplicator 
	UFUNCTION()
	void UpdateMultiplicator(bool Success);

private:

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
	//Updates the highlights of the keys
	UFUNCTION()
	void UpdateHighlight();
	//Checks the list of current target and assign the closest
	//one as the primary target
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

	UFUNCTION(BlueprintCallable)
	void WriteScore(FString Name);

	UFUNCTION(BlueprintCallable)
	static TArray<FKCD_PlayerScore> FetchScores();
	
private:



};
