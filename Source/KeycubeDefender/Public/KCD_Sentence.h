// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KCD_TypingStats.h"
#include "KVA_CubeVisual.h"
#include "GameFramework/Actor.h"
#include "KCD_Sentence.generated.h"

class UBoxComponent;
class UPaperSpriteComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSentenceComplete);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNewSentence);
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
	UPROPERTY(EditAnywhere, Category="mesh")
	UBoxComponent* Collision;
	

private :
	
	//Var for the letters to spawn
	UPROPERTY(EditAnywhere ,Category="Variables")
	TArray<FName> LettersInstances;

	UPROPERTY(EditAnywhere ,Category="Variables")
	int CurrentLetterIndex = 0;

	UPROPERTY(EditAnywhere, Category="Words")
	TArray<int> SentencesIndexUsed;
	
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

	//Set the current sentence
	UFUNCTION(BlueprintCallable)
	void SetSentence(FString Sentence);

	//Divides the sentence in an array of letters
	UFUNCTION(BlueprintCallable)
	void DivideLetters();

	//Tries to type the next letter. Returns if the hit was successful
	UFUNCTION(BlueprintCallable)
	bool Hit(FName Letter);

	//Deletes the last character entered
	UFUNCTION(BlueprintCallable)
	void Backspace();
	
	//Distance between the wanted sentence and what was written
	UFUNCTION(BlueprintCallable)
	int EditDistance();
	
	//Divides the sentence into individual words
	UFUNCTION(BlueprintCallable)
	TArray<FString> WordsFromString();

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

	//Highlight the current letter on the cube
	UFUNCTION()
	void HighlightCurrent();

	//Finishes up the test
	UFUNCTION()
	void UnhighlightCurrent();

	//Verifies if the cube is valid and tries to fetch it if it isn't
	UFUNCTION()
	bool VerifyCubeVisual();
	
	UPROPERTY()
	int SentenceNum = 0;

	UPROPERTY(EditAnywhere)
	int RequiredSentences = 5;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AKVA_CubeVisual* CubeInstance;

	//Tells the cube to send the typing feedback on the letter
	UFUNCTION()
	void KeyPress(FKey KeyParam);
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	UPROPERTY(BlueprintAssignable)
	FOnSentenceComplete OnSentenceCompleteDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnTestComplete OnTestCompleteDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnNewSentence OnNewSentenceDelegate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Words", meta = (ExposeOnSpawn=true))
	FString CurrentSentence;

	//Returns an average of the test's stats
	UFUNCTION(BlueprintCallable)
	FKCD_TypingStats AverageStats();

	//Returns the current index
	UFUNCTION(BlueprintCallable)
	int GetCurrentIndex(){return CurrentLetterIndex;}

	//Advances the index if it can and calls the feedback functions
	UFUNCTION(BlueprintCallable)
	void AdvanceIndex();
	

};