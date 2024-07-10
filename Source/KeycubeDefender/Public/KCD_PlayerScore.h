// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KCD_PlayerScore.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FKCD_PlayerScore : public FTableRowBase
{
  GENERATED_USTRUCT_BODY()
public:
	FKCD_PlayerScore():
	Score(0),
	Name("ABC")
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Score;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;
	
};
