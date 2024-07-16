// Fill out your copyright notice in the Description page of Project Settings.


#include "KCD_Sentence.h"

#include <iomanip>
#include <iostream>

#include "KCD_PlayerController.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetStringLibrary.h"

// Sets default values
AKCD_Sentence::AKCD_Sentence()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SentenceHolder = CreateDefaultSubobject<USceneComponent>("Sentence Holder");
	SentenceHolder->SetupAttachment(RootComponent);

	LetterMarker = CreateDefaultSubobject<UPaperSpriteComponent>("Marker");
	LetterMarker->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AKCD_Sentence::BeginPlay()
{
	Super::BeginPlay();

	//Get the current player controller
	AKCD_PlayerController* PlayerController = Cast<AKCD_PlayerController>(UGameplayStatics::GetPlayerController(
		this, 0));

	//Subscribes to the delegate for then a key is pressed and when it's released
	PlayerController->KeyPressDelegate.AddDynamic(this, &AKCD_Sentence::KeyPress);

	SetSentence("This is a test sentence, you are not supposed to see this");
	
}

void AKCD_Sentence::SetSentence(FString Sentence)
{
	CurrentLetterIndex = 0;
	
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

	//We spawn a KCD_Letter BP for each letters in the word 
	int x = 0;
	int y = 0;
	//For every words in the sentence
	for (auto Element : WordsFromString())
	{
		//Check if the word would be too big for the screen size
		if((Lettersize * (x + 0.5)) + (Lettersize * (Element.Len() + 0.5)) > ScreenSize)
		{
			x = 0;
			y++;
		}
		//for every letters in the word
		for (auto letter : UKismetStringLibrary::GetCharacterArrayFromString(Element))
		{
			//Position uses half the size of the word and offset that position
			//with the leght of th eprevious letters
			float yPosition = (ScreenSize/2) - (Lettersize * (x + 0.5));
			float zPosition = Lettersize * -(2 * y); 
			spawnTransform.SetLocation(FVector{0.0f, yPosition, zPosition});
			
			LettersInstances.Add(AddChildLetter(letter, spawnTransform));
			x++;
		}
		//Add a space at the end of the word
		float yPosition = (ScreenSize/2) - (Lettersize * (x + 0.5));
		float zPosition = Lettersize * -(2 * y); 
		spawnTransform.SetLocation(FVector{0.0f, yPosition, zPosition});
			
		LettersInstances.Add(AddChildLetter(" ", spawnTransform));
		x++;
	}
	//Remove the last element, which is a stray space
	LettersInstances.Last()->Destroy();
	LettersInstances.Pop();
	
}

bool AKCD_Sentence::Hit(FName Letter)
{
	UE_LOG(LogTemp, Warning, TEXT("Key Hit : %s"), *Letter.ToString());
	UE_LOG(LogTemp, Warning, TEXT("Key required : %s"), *LettersInstances[CurrentLetterIndex]->CurrentLetter.ToString());
	
	if(LettersInstances[CurrentLetterIndex]->CurrentLetter == ToHex(Letter.ToString()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit successfull"));
		
		LettersInstances[CurrentLetterIndex]->Hide();
		if(LettersInstances.Num() > CurrentLetterIndex + 1)
			CurrentLetterIndex++;
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("This is the end of the sentence"));
			WordComplete();
		}
		
		LettersInstances[CurrentLetterIndex]->PrimaryTargetHighlight();

		FVector markerLocation = LettersInstances[CurrentLetterIndex]->GetTransform().GetLocation();
		markerLocation.Z = markerLocation.Z - (Lettersize/2);
		
		LetterMarker->SetWorldLocation(markerLocation);
		return true;
	}

	UE_LOG(LogTemp, Warning, TEXT("Hit failed"));
	
	return false;
}

int AKCD_Sentence::EditDistance()
{

	return 0;
}

FString AKCD_Sentence::ToHex(FString Letter)
{
	//Transforms the character into an Hex value to fetch
	//the right sprite (Unreal doesn't allow special character as
	//Data table row name)
	std::string f = TCHAR_TO_UTF8(*Letter);
	char c = f[0];
	std::stringstream stream;

	stream << std::hex << std::setw(2) << std::setfill('0') <<
		(int) static_cast <unsigned char>(std::tolower(c));

	std::string result( stream.str() );
	FString resultFstring(result.c_str());

	return resultFstring;
}

TArray<FString> AKCD_Sentence::WordsFromString()
{
	TArray<FString> wordArray;
	std::string sentence = TCHAR_TO_UTF8(*CurrentSentence);
	size_t pos = 0;
	std::string word;
	while ((pos = sentence.find(" ")) != std::string::npos) {
		word = sentence.substr(0, pos);

		wordArray.Add(word.c_str());
		
		sentence.erase(0, pos + 1);
	}
	//Last word in the sentence
	wordArray.Add(sentence.c_str());

	return wordArray;
}

AKCD_Letters* AKCD_Sentence::AddChildLetter(FString Letter, FTransform SpawnTransform)
{
	//Add the letter as a child component of the sentence
	UChildActorComponent* child = NewObject<UChildActorComponent>(this);
	child->SetupAttachment(SentenceHolder);
	child->RegisterComponent();
	child->SetChildActorClass(LetterBP);
	child->SetRelativeTransform(SpawnTransform);
	//Get a reference to the child's class and set it's letter
	AKCD_Letters* letterObject = Cast<AKCD_Letters>(child->GetChildActor());
	if(letterObject != nullptr)
	{
		letterObject->SetLetter(FName(ToHex(Letter)));
	} else
	{
		UE_LOG(LogTemp, Warning, TEXT("Letter object reference NULL"));
	}
	
	return letterObject;
}

void AKCD_Sentence::KeyPress(FKey key)
{
	if(SpecialKeys.Contains(key))
	{
		Hit(FName(SpecialKeys[key]));
	} else
	{
		Hit(key.GetFName());
	}
	
}

void AKCD_Sentence::WordComplete()
{
	for (auto LettersInstance : LettersInstances)
	{
		LettersInstance->Destroy();
	}

	LettersInstances.Empty();

	SetSentence("This is the other test sentence for when you complete the first one");
}


// Called every frame
void AKCD_Sentence::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

