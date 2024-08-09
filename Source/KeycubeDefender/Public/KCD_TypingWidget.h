// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KCD_Sentence.h"
#include "Blueprint/UserWidget.h"
#include "KCD_TypingWidget.generated.h"

/**
 * 
 */

USTRUCT()
struct FLetter
{
	GENERATED_BODY()

	bool WasGood;
	FName Letter;
};

UCLASS()
class KEYCUBEDEFENDER_API UKCD_TypingWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AKCD_Sentence* SentenceInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ModifiedSentence;

	UPROPERTY()
	TMap<int, bool> LetterAssociation;

	//Fetches the sentence held in the sentence object
	UFUNCTION(BlueprintCallable)
	void NewSentence();

	//Feedback when a letter is typed 
	UFUNCTION(BlueprintCallable)
	void LetterFeedback(bool wasRight);

	//Feedback of deleting the previously typed letter
	UFUNCTION(BlueprintCallable)
	void BackspaceFeedback();

	//Removes the end marker at the specified index
	UFUNCTION(BlueprintCallable)
	FString RemoveEndMarker(int Index, FString Sentence);

	//Removes the end marker at the specified index
	UFUNCTION(BlueprintCallable)
	FString RemoveBeginningMarker(int Index, FString Sentence);

	//Returns the modified sentence
	UFUNCTION(BlueprintCallable)
	FString RefreshSentence();

	//Add "current target" feedback
	UFUNCTION(BlueprintCallable)
	void TargetLetter(int index);

	//Removes "current target" feedback
	UFUNCTION(BlueprintCallable)
	void RemoveTarget(int index);

	//Removes both beginning and ending markers
	UFUNCTION(BlueprintCallable)
	void RemoveMarker(int index);

private:
	UPROPERTY()
	int IndexOffset = 0;

	UPROPERTY()
	FString STYLE_RIGHT = "<Right>";
	UPROPERTY()
	FString STYLE_WRONG = "<Wrong>";
	UPROPERTY()
	FString STYLE_END = "</>";
	UPROPERTY()
	FString STYLE_CURRENT = "<Current>";

	UFUNCTION(BlueprintCallable)
	FString GetMarker(bool wasRight)
	{
		return (wasRight) ? STYLE_RIGHT : STYLE_WRONG;
	}
};
