// Fill out your copyright notice in the Description page of Project Settings.


#include "KCD_Letters.h"

#include <iomanip>

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
	
	FKCD_LetterAssociation* LetterAssociation = SpritesTable->FindRow<FKCD_LetterAssociation>(FName(ToHex(CurrentLetter.ToString())), "");
	if(LetterAssociation != nullptr)
	{
		return LetterAssociation->LetterSprite;
	}
	return nullptr;
}

FString AKCD_Letters::ToHex(FString Letter)
{
	//Transforms the character into an Hex value to fetch
	//the right sprite (Unreal doesn't allow special character as
	//Data table row name)
	std::string f = TCHAR_TO_UTF8(*Letter);
	char c = f[0];
	std::stringstream stream;

	stream << std::hex << std::setw(2) << std::setfill('0') <<
		(int)static_cast<unsigned char>(std::tolower(c));

	std::string result(stream.str());
	FString resultFstring(result.c_str());

	return resultFstring;
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

void AKCD_Letters::PrimaryTargetHighlight()
{
	Sprite->SetSpriteColor(PrimaryTargetColor);
}

void AKCD_Letters::Unhighlight()
{
	Sprite->SetSpriteColor(BaseColor);
	IsLetterHidden = false;
}

void AKCD_Letters::Hide()
{
	Sprite->SetSpriteColor(HiddenColor);
	IsLetterHidden = true;
}

void AKCD_Letters::ErrorHighlight()
{
	Sprite->SetSpriteColor(ErrorColor);
	IsLetterHidden = false;
}

