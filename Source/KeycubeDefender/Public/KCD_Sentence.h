// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KCD_Letters.h"
#include "KCD_TypingStats.h"
#include "KVA_CubeVisual.h"
#include "GameFramework/Actor.h"
#include "KCD_Sentence.generated.h"

class UBoxComponent;
class UPaperSpriteComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSentenceComplete);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTestComplete);
UCLASS()
class KEYCUBEDEFENDER_API AKCD_Sentence : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AKCD_Sentence();
	
	UPROPERTY(EditAnywhere, Category="Holder")
	USceneComponent* ActorCenter;
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
	FString TotalTypeSentence;

	UPROPERTY(EditAnywhere, Category="Words")
	FString TotalTypeInput;
	
	//Time the sentence was started at
	UPROPERTY()
	double StartTime = 0;
	//Time the sentence was started at
	UPROPERTY()
	double LastInputTime = 0;

	UPROPERTY()
	int Mistakes = 0;
	
	//If the user has already started typing
	UPROPERTY()
	bool HasStarted = false;


	//Stats for each sentences typed
	UPROPERTY()
	TArray<FKCD_TypingStats> Stats;

	//Set the word associated with the ship
	UFUNCTION(BlueprintCallable)
	void SetSentence(FString Sentence);

	//Spawn the letters for the associated word
	UFUNCTION(BlueprintCallable)
	void SpawnLetters();

	//Tries to type the next letter. Returns if the hit was successful
	UFUNCTION(BlueprintCallable)
	bool Hit(FName Letter);

	//Deletes the last character entered
	UFUNCTION(BlueprintCallable)
	void Backspace();

	//Moves the letter indicator to the current letter
	UFUNCTION()
	void MoveMarker();
	
	//Distance between the wanted sentence and what was written
	UFUNCTION(BlueprintCallable)
	int EditDistance();

	//Transforms the letter to hex code
	UFUNCTION(BlueprintCallable)
	FString ToHex(FString letter);

	//Divides the sentence into individual words
	UFUNCTION(BlueprintCallable)
	TArray<FString> WordsFromString();

	//Spawns the letters sprites as child objects
	UFUNCTION(BlueprintCallable)
	AKCD_Letters* AddChildLetter(FString Letter, FTransform SpawnTransform);

	//Confirms the completion of the sentence and cleans up variables for the next one
	UFUNCTION(BlueprintCallable)
	void SentenceComplete();

	//Fetches a random sentence in the sentence file
	UFUNCTION(BlueprintCallable)
	FString FetchNewSentence();

	//Writes the specified stat to the stat file
	UFUNCTION()
	void WriteStats(FString RowName, FKCD_TypingStats Stat);

	//Finishes up the test
	UFUNCTION()
	void TestOver();
	
	UPROPERTY()
	int SentenceNum = 0;

	UPROPERTY(EditAnywhere)
	int RequiredSentences = 5;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AKVA_CubeVisual* CubeInstance;
	
	UFUNCTION()
	void KeyPress(FKey KeyParam);
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	UPROPERTY(BlueprintAssignable)
	FOnSentenceComplete OnSentenceCompleteDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnTestComplete OnTestCompleteDelegate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Words", meta = (ExposeOnSpawn=true))
	FString CurrentSentence;

	UFUNCTION(BlueprintCallable)
	FKCD_TypingStats AverageStats();

	UFUNCTION(BlueprintCallable)
	int GetCurrentIndex(){return CurrentLetterIndex;}

	UFUNCTION(BlueprintCallable)
	void AdvanceIndex();
	

};