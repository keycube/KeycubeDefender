// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KCD_TypingStats.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FKCD_TypingStats : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	FKCD_TypingStats() :
		Score(0),
		TimeTaken(0),
		WordSize(0),
		Mistakes(0),
		WordDistance(0),
		WasAltTarget(true),
		WantedSentence(""),
		TypedSentence(""),
		Keystrokes("")
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Score;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	double TimeTaken;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	double WordSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	double Mistakes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	double WordDistance;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool WasAltTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString WantedSentence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString TypedSentence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Keystrokes;
};
