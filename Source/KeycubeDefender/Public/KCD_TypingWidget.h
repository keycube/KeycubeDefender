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
	
	UFUNCTION(BlueprintCallable)
	void NewSentence();
	
	UFUNCTION(BlueprintCallable)
	void LetterFeedback(bool wasRight);

	UFUNCTION(BlueprintCallable)
	FString RemoveEndMarker(int Index, FString Sentence);

	UFUNCTION(BlueprintCallable)
	void RemoveBeginningMarker(int Index);
	
	UFUNCTION(BlueprintCallable)
	FString RefreshSentence();

private:
	UPROPERTY()
	int IndexOffset = 0;

	UPROPERTY()
	FString STYLE_RIGHT = "<Right>";
	UPROPERTY()
	FString STYLE_WRONG = "<Wrong>";
	UPROPERTY()
	FString STYLE_END = "</>";

	UFUNCTION(BlueprintCallable)
	FString GetMarker(bool wasRight)
	{
		return (wasRight) ? STYLE_RIGHT : STYLE_WRONG;
	}
};
