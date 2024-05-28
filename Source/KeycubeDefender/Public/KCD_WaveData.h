// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/UserDefinedStruct.h"
#include "KCD_WaveData.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FKCD_WaveData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
	
	public:
    	FKCD_WaveData() :
    	SpawnSpeed(1.0),
    	NumberOfT0(0),
    	NumberOfT1(0),
    	NumberOfT2(0),
    	SpeedModifier(1.0)
    	{}
    
    	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    	float SpawnSpeed;
    
    	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    	int NumberOfT0;
    	
    	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    	int NumberOfT1;
    	
    	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    	int NumberOfT2;
    
    	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    	float SpeedModifier;
	
};
