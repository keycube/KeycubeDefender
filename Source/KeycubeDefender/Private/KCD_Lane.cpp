// Fill out your copyright notice in the Description page of Project Settings.


#include "KCD_Lane.h"

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

// Called every frame
void AKCD_Lane::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

