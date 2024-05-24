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


	
	UPROPERTY(EditAnywhere, Category="Sprite")
	UPaperSpriteComponent* Sprite;
	UPROPERTY(EditAnywhere, Category="Variable")
	FString CurrentLetter = "A";

	UPROPERTY(EditAnywhere, Category="Variable")
	FLinearColor HighlightColor;
	UPROPERTY(EditAnywhere, Category="Variable")
	FLinearColor BaseColor;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category="Variable")
	UDataTable* SpritesTable;

	UFUNCTION(BlueprintCallable)
	void SetSprite();

	UFUNCTION(BlueprintCallable)
	UPaperSprite* GetSpriteFromTable();

public:	
	UFUNCTION(BlueprintCallable)
	void SetLetter(FString letter);

	UFUNCTION(BlueprintCallable)
	void Highlight();

	UFUNCTION(BlueprintCallable)
	void Unhighlight();
		
};
