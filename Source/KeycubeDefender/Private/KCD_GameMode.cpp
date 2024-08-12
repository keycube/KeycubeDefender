// Fill out your copyright notice in the Description page of Project Settings.


#include "KCD_GameMode.h"

AKCD_WaveManager* AKCD_GameMode::GetShipSpawner()
{
	return ShipSpawner;
}

AKCD_LaneHolder* AKCD_GameMode::GetLaneHolder()
{
	return LaneHolder;
}

AKVA_CubeSpawner* AKCD_GameMode::GetCubeSpawner()
{
	if(CubeSpawner != nullptr)
	{
		return  CubeSpawner;
	}

	CubeSpawner = Cast<AKVA_CubeSpawner>(UGameplayStatics::GetActorOfClass(this, AKVA_CubeSpawner::StaticClass()));

	return CubeSpawner;
}

