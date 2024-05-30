// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KCD_Ship.h"
#include "KCD_Ship_Multitarget.generated.h"

/**
 * 
 */
UCLASS()
class KEYCUBEDEFENDER_API AKCD_Ship_Multitarget : public AKCD_Ship
{
	GENERATED_BODY()

public:
	
	virtual bool Hit(FName Letter) override;
};
