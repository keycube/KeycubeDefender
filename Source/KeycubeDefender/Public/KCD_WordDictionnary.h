// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KCD_Words.h"
#include "KCD_WordDictionnary.generated.h"

/**
 * 
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
	TArray<FKCD_Words> WordList;
};