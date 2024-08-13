#include "KVA_Keys.h"

AKVA_Keys::AKVA_Keys()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	KeyMesh = CreateDefaultSubobject<UStaticMeshComponent>("KeyMesh");
	RootComponent = KeyMesh;
	KeyText = CreateDefaultSubobject<UTextRenderComponent>("KeyText");
	KeyText->SetupAttachment(KeyMesh);
}

void AKVA_Keys::BeginPlay()
{
	AActor::BeginPlay();

	KeyText->SetText(AssociatedKey.GetDisplayName(false));

	DynamicMat = KeyMesh->CreateDynamicMaterialInstance(0, BaseMaterial);
}

void AKVA_Keys::KeyPressed_Keys()
{
	DynamicMat->SetVectorParameterValue("Color", PressedColor);
	IsPressed = true;
}

void AKVA_Keys::KeyReleased_Keys()
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

void AKVA_Keys::HighlightKey()
{
	IsHighlighted = true;
	if(!IsPressed)
	{
		DynamicMat->SetVectorParameterValue("Color", HighlighColor);
	}
}

void AKVA_Keys::UnhighlightKey()
{
	IsHighlighted = false;
	if(!IsPressed)
	{
		DynamicMat->SetVectorParameterValue("Color", BaseColor);
	}
}

void AKVA_Keys::ChangeKey(FKey NewKey)
{
	if(NewKey == FKey("None"))
	{
		AssociatedKey = NewKey;
		KeyText->SetText(FText::FromString(""));
		HighlightKey();
	}
	
	AssociatedKey = NewKey;
	KeyText->SetText(NewKey.GetDisplayName(false));
	HighlightKey();
}
