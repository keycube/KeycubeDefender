// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperSpriteComponent.h"
#include "Components/ActorComponent.h"
#include "KCD_Letters.generated.h"


UCLASS()
class KEYCUBEDEFENDER_API AKCD_Letters : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	AKCD_Letters();

	//Variable for the visual of the object
	UPROPERTY(EditAnywhere, Category="Sprite")
	UPaperSpriteComponent* Sprite;
	UPROPERTY(EditAnywhere, Category="Variable")
	FName CurrentLetter = "A";

	//Color of the feedback when the letter is next in the target's letters to hit
	UPROPERTY(EditAnywhere, Category="Variable")
	FLinearColor HighlightColor;
	UPROPERTY(EditAnywhere, Category="Variable")
	FLinearColor PrimaryTargetColor;
	UPROPERTY(EditAnywhere, Category="Variable")
	FLinearColor BaseColor;
	UPROPERTY(EditAnywhere, Category="Variable")
	FLinearColor HiddenColor;
	UPROPERTY(EditAnywhere, Category="Variable")
	FLinearColor ErrorColor;

	UPROPERTY(EditAnywhere, Category="Variable")
	bool IsLetterHidden;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	//Data table containing associations between letters and sprites
	UPROPERTY(EditAnywhere, Category="Variable")
	UDataTable* SpritesTable;

	//Sets the sprite of the letter
	UFUNCTION(BlueprintCallable)
	void SetSprite();

	//Uses the SpritesTable to find the current letter association
	UFUNCTION(BlueprintCallable)
	UPaperSprite* GetSpriteFromTable();

	//Transforms the letter to hex code
	UFUNCTION(BlueprintCallable)
	FString ToHex(FString letter);

public:
	//Sets the letter
	UFUNCTION(BlueprintCallable)
	void SetLetter(FName letter);

	//Visual feedback to make the letter easier to see
	UFUNCTION(BlueprintCallable)
	void Highlight();

	//Visual feedback to make the letter easier to see
	UFUNCTION(BlueprintCallable)
	void PrimaryTargetHighlight();

	//Resets the letter to it's default color
	UFUNCTION(BlueprintCallable)
	void Unhighlight();

	//Visual feedback to hide the letter
	UFUNCTION(BlueprintCallable)
	void Hide();

	//Visual feedback of a wrong letter typed
	UFUNCTION(BlueprintCallable)
	void ErrorHighlight();
		
};
