#pragma once
#include "KVA_Keys.h"

#include "KVA_CubeVisual.generated.h"

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

private:
	
	//Fill the map of association between the inputs and the
	//keys on the cube
	UFUNCTION()
	void FillKeyMap();

	//Updates the highlights of the keys
	UFUNCTION()
	void UpdateHighlight();
};
