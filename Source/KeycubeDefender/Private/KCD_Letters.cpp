// Fill out your copyright notice in the Description page of Project Settings.


#include "KCD_Letters.h"

#include "KCD_LetterAssociation.h"

// Sets default values for this component's properties
AKCD_Letters::AKCD_Letters()
{
	PrimaryActorTick.bCanEverTick = false;

	Sprite = CreateDefaultSubobject<UPaperSpriteComponent>("LetterSprite");
	RootComponent = Sprite;
}


// Called when the game starts
void AKCD_Letters::BeginPlay()
{
	Super::BeginPlay();
	
}

void AKCD_Letters::SetSprite()
{
	Sprite->SetSprite(GetSpriteFromTable());
}

UPaperSprite* AKCD_Letters::GetSpriteFromTable()
{
	
	FKCD_LetterAssociation* LetterAssociation = SpritesTable->FindRow<FKCD_LetterAssociation>(CurrentLetter, "");
	if(LetterAssociation->LetterSprite != nullptr)
	{
		return LetterAssociation->LetterSprite;
	}
	return nullptr;
}

void AKCD_Letters::SetLetter(FName letter)
{
	CurrentLetter = letter;

	SetSprite();
}

void AKCD_Letters::Highlight()
{
	Sprite->SetSpriteColor(HighlightColor);
}

void AKCD_Letters::Unhighlight()
{
	Sprite->SetSpriteColor(BaseColor);
	IsLetterHidden = false;
}

void AKCD_Letters::Hide()
{
	Sprite->SetSpriteColor(FLinearColor(255,255,255, 0.2));
	IsLetterHidden = true;
}

