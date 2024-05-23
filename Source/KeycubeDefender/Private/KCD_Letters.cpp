// Fill out your copyright notice in the Description page of Project Settings.


#include "KCD_Letters.h"

#include "KCD_LetterAssociation.h"

// Sets default values for this component's properties
AKCD_Letters::AKCD_Letters()
{
	PrimaryActorTick.bCanEverTick = false;

	Sprite = CreateDefaultSubobject<UPaperSpriteComponent>("LetterSprite");
	Sprite->SetupAttachment(RootComponent);
}


// Called when the game starts
void AKCD_Letters::BeginPlay()
{
	Super::BeginPlay();
	
}

void AKCD_Letters::SetSprite()
{
	UE_LOG(LogTemp, Warning, TEXT("In the SetSprite"));
	
	Sprite->SetSprite(GetSpriteFromTable());
	
}

UPaperSprite* AKCD_Letters::GetSpriteFromTable()
{
	
	FKCD_LetterAssociation* LetterAssociation = SpritesTable->FindRow<FKCD_LetterAssociation>(FName(CurrentLetter), "");
	if(LetterAssociation->LetterSprite != nullptr)
	{
		return LetterAssociation->LetterSprite;
	}
	return nullptr;
}

void AKCD_Letters::SetLetter(FString letter)
{
	CurrentLetter = letter;

	SetSprite();
}

