// Fill out your copyright notice in the Description page of Project Settings.


#include "KCD_Keys.h"

// Sets default values
AKCD_Keys::AKCD_Keys()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

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

void AKCD_Keys::KeyPressed_Keys()
{
	DynamicMat->SetVectorParameterValue("Color", PressedColor);
	IsPressed = true;
}

void AKCD_Keys::KeyReleased_Keys()
{
	IsPressed = false;
	if(!IsHighlighted)
	{
		DynamicMat->SetVectorParameterValue("Color", BaseColor);
	} else
	{
		HighlightKey();
	}
}

void AKCD_Keys::HighlightKey()
{
	IsHighlighted = true;
	if(!IsPressed)
	{
		DynamicMat->SetVectorParameterValue("Color", HighlighColor);
	} 
	
}

void AKCD_Keys::UnhighlightKey()
{
	IsHighlighted = false;
	if(!IsPressed)
	{
		DynamicMat->SetVectorParameterValue("Color", BaseColor);
	}
}

