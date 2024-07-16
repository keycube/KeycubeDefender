// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KCD_Letters.h"
#include "GameFramework/Actor.h"
#include "KCD_Sentence.generated.h"

class UBoxComponent;
class UPaperSpriteComponent;

UCLASS()
class KEYCUBEDEFENDER_API AKCD_Sentence : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AKCD_Sentence();

	UPROPERTY(EditAnywhere, Category="Holder")
	USceneComponent* SentenceHolder;
	UPROPERTY(EditAnywhere, Category="Holder")
	UPaperSpriteComponent* LetterMarker;
	UPROPERTY(EditAnywhere, Category="mesh")
	UBoxComponent* Collision;
	

private :
	//Var for the letters to spawn
	UPROPERTY(EditAnywhere ,Category="Variables")
	TArray<AKCD_Letters*> LettersInstances;
	UPROPERTY(EditAnywhere ,Category="Variables")
	TSubclassOf<AKCD_Letters> LetterBP;

	UPROPERTY(EditAnywhere ,Category="Variables")
	int CurrentLetterIndex = 0;

	UPROPERTY(EditAnywhere, Category="Words")
	TArray<int> SentencesIndexUsed;

	//This var is used to format the words to the screen
	UPROPERTY(EditAnywhere, Category="Words")
	float Lettersize = 30.0;

	UPROPERTY(EditAnywhere, Category="Words")
	float ScreenSize = 500.0;
	
	UPROPERTY(EditAnywhere, Category="Words")
	FString TotalTypeWord;

	UPROPERTY(EditAnywhere, Category="Words")
	TMap<FKey, FString> SpecialKeys;
	
	//Time the sentence was started at
	UPROPERTY()
	double StartTime = 0;

	//Set the word associated with the ship
	UFUNCTION(BlueprintCallable)
	void SetSentence(FString Sentence);

	//Spawn the letters for the associated word
	UFUNCTION(BlueprintCallable)
	void SpawnLetters();

	//Tries to type the next letter. Returns if the hit was successful
	UFUNCTION(BlueprintCallable)
	virtual bool Hit(FName Letter);
	
	//Set the word associated with the ship
	UFUNCTION(BlueprintCallable)
	int EditDistance();

	//Transforms the letter to hex code
	UFUNCTION(BlueprintCallable)
	FString ToHex(FString letter);

	//Transforms the letter to hex code
	UFUNCTION(BlueprintCallable)
	TArray<FString> WordsFromString();

	//Transforms the letter to hex code
	UFUNCTION(BlueprintCallable)
	AKCD_Letters* AddChildLetter(FString Letter, FTransform SpawnTransform);

	UFUNCTION(BlueprintCallable)
	void KeyPress(FKey key);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Words", meta = (ExposeOnSpawn=true))
	FString CurrentSentence;

};