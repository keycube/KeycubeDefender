// Fill out your copyright notice in the Description page of Project Settings.


#include "KCD_Keys.h"

// Sets default values
AKCD_Keys::AKCD_Keys()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	KeyMesh = CreateDefaultSubobject<UStaticMeshComponent>("KeyMesh");
	RootComponent = KeyMesh;
	KeyText = CreateDefaultSubobject<UTextRenderComponent>("KeyText");
	KeyText->SetupAttachment(KeyMesh);

	

}

// Called when the game starts or when spawned
void AKCD_Keys::BeginPlay()
{
	Super::BeginPlay();

	KeyText->SetText(AssociatedKey.GetDisplayName(false));

	DynamicMat = KeyMesh->CreateDynamicMaterialInstance(0, BaseMaterial);
	
	
}

// Called every frame
void AKCD_Keys::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AKCD_Keys::KeyPressed_Keys()
{
	DynamicMat->SetVectorParameterValue("Color", PressedColor);
}

void AKCD_Keys::KeyReleased_Keys()
{
	DynamicMat->SetVectorParameterValue("Color", BaseColor);
}

