// Fill out your copyright notice in the Description page of Project Settings.


#include "KCD_Lane.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
AKCD_Lane::AKCD_Lane()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>("LaneRoot");

	RootComponent = SceneComponent;
}

// Called when the game starts or when spawned
void AKCD_Lane::BeginPlay()
{
	Super::BeginPlay();
	
}

AKCD_Ship* AKCD_Lane::SpawnShip(TSubclassOf<AKCD_Ship> Ship, int WordIndex, FString Word, float SpeedModifier)
{
	if(!Ship->IsValidLowLevel())
	{
		return nullptr;
	}
	
	FTransform spawnTransform{
		this->GetTransform().GetRotation().Rotator() + FRotator{0.0f, 180.0f, 0.0f}, // Rotation
		this->GetTransform().GetLocation(), // Translation
		FVector{1.0f, 1.0f, 1.0f} // Scale
	};

	AKCD_Ship* ShipSpawned;
	
	ShipSpawned = GetWorld()->SpawnActorDeferred<AKCD_Ship>(Ship, spawnTransform);

	//Setting the ship's variables
	ShipSpawned->Initialize(Word, WordIndex, SpeedModifier);

	UGameplayStatics::FinishSpawningActor(ShipSpawned, spawnTransform);
	
	return ShipSpawned;
}

// Called every frame
void AKCD_Lane::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

