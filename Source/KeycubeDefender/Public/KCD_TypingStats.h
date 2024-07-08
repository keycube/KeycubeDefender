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
		WasAltTarget(false)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Score;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int WordSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	double TimeTaken;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Mistakes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int WordDistance;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool WasAltTarget;
};
