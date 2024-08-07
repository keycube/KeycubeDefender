// Fill out your copyright notice in the Description page of Project Settings.


#include "KVA_CubeSpawner.h"

#include "KVA_Cube_SaveType.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AKVA_CubeSpawner::AKVA_CubeSpawner()
{
	
}

// Called when the game starts or when spawned
void AKVA_CubeSpawner::BeginPlay()
{
	Super::BeginPlay();

	ChangeCubeType();
}

TSubclassOf<AKVA_CubeVisual> AKVA_CubeSpawner::FetchCubeType()
{
	if(UKVA_Cube_SaveType* CubeSave = Cast<UKVA_Cube_SaveType>(UGameplayStatics::LoadGameFromSlot(UKVA_Cube_SaveType::SlotName, 0)))
	{
		return CubeSave->CubeType;
	}
	return nullptr;
}

void AKVA_CubeSpawner::ChangeCubeType()
{
	TSubclassOf<AKVA_CubeVisual> selectedCube = FetchCubeType();

	FActorSpawnParameters SpawnInfo;
	
	Cube = GetWorld()->SpawnActor<AKVA_CubeVisual>(selectedCube, this->GetTransform(), SpawnInfo);
	//Unreal has a bug here, making the spawn ignore the scale
	//so we adjust it manually
	Cube->SetActorScale3D(GetActorScale());

	CubeSpawnedDelegate.Broadcast();
}

