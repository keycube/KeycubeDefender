// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KCD_WordDictionnary.generated.h"

/**
 * Structure containing lists of words and the tier they belong to
 */
USTRUCT(BlueprintType)
struct FKCD_WordDictionnary : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	FKCD_WordDictionnary() :
	Tier(0),
	WordList()
	{}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Tier;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> WordList;
};
