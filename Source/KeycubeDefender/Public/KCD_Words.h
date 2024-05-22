// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/UserDefinedStruct.h"
#include "KCD_Words.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FKCD_Words : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:

	FKCD_Words()
		: IsUsed(false)
		, Word("")
	{}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsUsed;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Word;
};
