// Fill out your copyright notice in the Description page of Project Settings.


#include "KCD_Sentence.h"

#include <iomanip>
#include <iostream>

#include "Kismet/KismetStringLibrary.h"

// Sets default values
AKCD_Sentence::AKCD_Sentence()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SentenceHolder = CreateDefaultSubobject<USceneComponent>("Sentence Holder");
	RootComponent = SentenceHolder;

	LetterMarker = CreateDefaultSubobject<UPaperSpriteComponent>("Marker");
	LetterMarker->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AKCD_Sentence::BeginPlay()
{
	Super::BeginPlay();

	SetSentence("This is a test sentence, you are not supposed to see this");

	//SetSentence("qwertyuiop[]asdfghjkl;'zxcvbnm,./1234567890-= <>?\":{}+_)(*&^%$#@!");
	
}

void AKCD_Sentence::SetSentence(FString Sentence)
{
	CurrentSentence = Sentence;

	SpawnLetters();
	
}

void AKCD_Sentence::SpawnLetters()
{
	//Base transform of the letters, used to spawn the letter in local position
	FTransform spawnTransform{
		FRotator{0.0f, -90.0f, 0.0f},                 // Rotation
		FVector{0.0f, 0.0f, 0.0f},  // Translation
		FVector{1.0f, 1.0f, 1.0f}   // Scale
	};
	//Total size of the word
	const float WordSize = Lettersize * CurrentSentence.Len();

	//TODO : If the sentence is too big, make it appear on multiple lines 
	
	//We spawn a KCD_Letter BP for each letters in the word 
	int x = 0;
	for (auto letter : UKismetStringLibrary::GetCharacterArrayFromString(CurrentSentence))
	{
		//Position uses half the size of the word and offset that position
		//with the leght of theprevious letters
		float yPosition = (WordSize/2) - (Lettersize * (x + 0.5));
		spawnTransform.SetLocation(FVector{0.0f, yPosition, 0.0f});

		//Add the letter as a child component of the ship
		UChildActorComponent* child = NewObject<UChildActorComponent>(this);
		child->SetupAttachment(SentenceHolder);
		child->RegisterComponent();
		child->SetChildActorClass(LetterBP);
		child->SetRelativeTransform(spawnTransform);
		//Get a reference to the child's class and set it's letter
		AKCD_Letters* letterObject = Cast<AKCD_Letters>(child->GetChildActor());
		if(letterObject != nullptr)
		{
			//Transforms the character into an Hex value to fetch
			//the right sprite (Unreal doesn't allow special character as
			//Data table row name)
			std::string f = TCHAR_TO_UTF8(*letter);
			char c = f[0];
			std::stringstream stream;
			stream << std::hex << std::setw(2) << std::setfill('0') <<
				(int) static_cast <unsigned char>(std::tolower(c));
			std::string result( stream.str() );
			FString resultFstring(result.c_str());
			
			letterObject->SetLetter(FName(resultFstring));
		} else
		{
			UE_LOG(LogTemp, Warning, TEXT("Letter object reference NULL"));
		}
	
		LettersInstances.Add(letterObject);
		x++;
	}
}

bool AKCD_Sentence::Hit(FName Letter)
{
	return false;
}

int AKCD_Sentence::EditDistance()
{

	return 0;
}


// Called every frame
void AKCD_Sentence::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

