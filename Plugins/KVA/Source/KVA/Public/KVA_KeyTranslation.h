// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KVA_KeyTranslation.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FKVA_KeyTranslation : public FTableRowBase
{
 GENERATED_USTRUCT_BODY()

public:
 FKVA_KeyTranslation() :
 Key()
 {}
 
 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Key)
 FKey Key;
};

