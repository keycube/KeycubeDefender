// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/UserDefinedStruct.h"
#include "PaperSprite.h"
#include "KCD_LetterAssociation.generated.h"

/**
 * Structure containing an association of letters to the sprite of the letter
 */
USTRUCT(BlueprintType)
struct FKCD_LetterAssociation : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	FKCD_LetterAssociation() :
	LetterSprite()
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPaperSprite* LetterSprite;

	
	
};
