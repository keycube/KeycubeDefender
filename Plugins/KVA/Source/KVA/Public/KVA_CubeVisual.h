#pragma once
#include "KVA_Keys.h"
#include <iostream>
#include <fstream>
#include "KVA_CubeVisual.generated.h"

USTRUCT()
struct FKeyRow
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<AKVA_Keys*> Keys;

	AKVA_Keys* operator[] (int32 i)
	{
		return Keys[i];
	}
};

USTRUCT()
struct FKeyFace
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<FKeyRow> Face;

	FKeyRow operator[] (int32 i)
	{
		return Face[i];
	}
};

UCLASS()
class KVA_API AKVA_CubeVisual : public AActor
{
	GENERATED_BODY()
	
public:
	AKVA_CubeVisual();
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Map of association between the inputs and the
	//keys on the cube
	UPROPERTY(VisibleAnywhere, Category=Keys)
	TMap<FKey, AKVA_Keys*> Keys;

	//Map of association between the inputs and the
	//keys on the cube
	UPROPERTY(EditAnywhere, Category=Keys)
	TArray<FKeyFace> KeysMatrix;

	//Map of keys with the Highlight state
	UPROPERTY(VisibleAnywhere, Category=Keys)
	TArray<FKey> HighlitedKeys;
	
	//Mark the letter as pressed
	UFUNCTION(BlueprintCallable)
	void KeyPressed(FKey KeyToPress);

	//Mark the letters who would result in a good hit
	UFUNCTION(BlueprintCallable)
	void KeyReleased(FKey KeyToRelease);

	//Mark the letters who would result in a good hit
	UFUNCTION(BlueprintCallable)
	void HighlightKeys(TArray<FKey> keysToHighlight);

	//Mark the letters who would result in a good hit
	UFUNCTION(BlueprintCallable)
	void UnhighlightKeys(TArray<FKey> keysToHighlight);

	//Fill the map of association between the inputs and the
	//keys on the cube
	UFUNCTION(BlueprintCallable)
	void FillKeyMatrix(TArray<UChildActorComponent*> KeyActors, int Row, int Face);

	//Saves the current key matrix to be loaded next time
	UFUNCTION(BlueprintCallable)
	void SaveKeyMatrix();

	//Calls the change key function of the key actor and refreshes the key map
	UFUNCTION(BlueprintCallable)
	void ChangeKey(AKVA_Keys* ChangingKey, FKey NewKey);

private:
	
	//Fill the map of association between the inputs and the
	//keys on the cube
	UFUNCTION()
	void FillKeyMap();

	//Updates the highlights of the keys
	UFUNCTION()
	void UpdateHighlight();

	//Load the saved data of the letters matrix
	UFUNCTION()
	void LoadKeyMatrix();

	UFUNCTION(BlueprintCallable)
	void WriteToFile();
	
	UFUNCTION(BlueprintCallable)
	void ReadFile();
};
