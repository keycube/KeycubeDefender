// Fill out your copyright notice in the Description page of Project Settings.


#include "KCD_Ship.h"

#include "KCD_GameMode.h"
#include "KCD_WaveManager.h"
#include "PaperSprite.h"
#include "Kismet/GameplayStatics.h"
#include "Components/ChildActorComponent.h"
#include "Kismet/KismetStringLibrary.h"

// Sets default values
AKCD_Ship::AKCD_Ship()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Add the components of the ship
	Collision = CreateDefaultSubobject<UBoxComponent>("BoxCollision");
	RootComponent = Collision;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("ShipMesh");
	Mesh->SetupAttachment(RootComponent);
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ShipMovement");

	LettersHolder = CreateDefaultSubobject<USceneComponent>("LetterHolder");
	LettersHolder->SetupAttachment(RootComponent);
	
	LettersBackground = CreateDefaultSubobject<UPaperSpriteComponent>("LettersBackground");
	LettersBackground->SetupAttachment(LettersHolder);

	SubTargetSprite = CreateDefaultSubobject<UPaperSpriteComponent>("SubTargetMarker");
	SubTargetSprite->SetupAttachment(RootComponent);

	MainTargetSprite = CreateDefaultSubobject<UPaperSpriteComponent>("MainTargetMarker");
	MainTargetSprite->SetupAttachment(RootComponent);
}

void AKCD_Ship::Initialize(FString NewWord, int NewWordIndex, float SpeedModifier)
{
	//Settings of values used for the ship's spawn and behavior
	SetWord(NewWord);
	WordIndex = NewWordIndex;
	SetShipSpeed(SpeedModifier);

	SubTargetSprite->SetVisibility(false);
	MainTargetSprite->SetVisibility(false);
}

// Called when the game starts or when spawned
void AKCD_Ship::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AKCD_Ship::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AKCD_Ship::SetWord(FString word)
{
	CurrentWord = word;

	Stats.WordSize = GetNum(CurrentWord);
	
	SpawnLetters();

	CurrentLetter = LettersInstances[CurrentLetterIndex]->CurrentLetter;
}

int AKCD_Ship::EditDistance()
{
	int m = GetNum(CurrentWord);
	int n = GetNum(TotalTypeWord);

	std::string s = TCHAR_TO_UTF8(*CurrentWord);
	std::string t = TCHAR_TO_UTF8(*TotalTypeWord);
	
	std::vector<int> prev(n + 1, 0), curr(n + 1, 0);

	for (int j = 0; j <= n; j++) {
		prev[j] = j;
	}

	for (int i = 1; i <= m; i++) {
		curr[0] = i;
		for (int j = 1; j <= n; j++) {
			if (s[i - 1] == t[j - 1]) {
				curr[j] = prev[j - 1];
			}
			else {
				int mn
					= std::min(1 + prev[j], 1 + curr[j - 1]);
				curr[j] = std::min(mn, 1 + prev[j - 1]);
			}
		}
		prev = curr;
	}

	return prev[n];
}

void AKCD_Ship::Targeted()
{
	LettersInstances[0]->Highlight();
}

void AKCD_Ship::SpawnLetters()
{
	//Base transform of the letters, used to spawn the letter in local position
	FTransform spawnTransform{
		FRotator{0.0f, -90.0f, 0.0f},                 // Rotation
		FVector{25.0f, -20.0f, 0.0f},  // Translation
		FVector{1.0f, 1.0f, 1.0f}   // Scale
	};
	//Total size of the word
	const float WordSize = Lettersize * CurrentWord.Len();

	//Base transform of the letters, used to spawn the letter in local position
	FTransform BackgroundTransform{
		FRotator{0.0f, -90.0f, 0.0f},                 // Rotation
		FVector{0.0f, 0, 0.5},  // Translation
		FVector{(2.0f * CurrentWord.Len()) + 1.5f, 2.5f, 2.5f}   // Scale
	};

	LettersBackground->SetRelativeTransform(BackgroundTransform);

	//We spawn a KCD_Letter BP for each letters in the word 
	int x = 0;
	for (auto letter : UKismetStringLibrary::GetCharacterArrayFromString(CurrentWord))
	{
		//Position uses half the size of the word and offset that position
		//with the leght of theprevious letters
		float yPosition = (WordSize/2) - (Lettersize * (x + 0.5));
		spawnTransform.SetLocation(FVector{0.0f, yPosition, 0.0f});

		//Add the letter as a child component of the ship
		UChildActorComponent* child = NewObject<UChildActorComponent>(this);
		child->SetupAttachment(LettersHolder);
		child->RegisterComponent();
		child->SetChildActorClass(LetterBP);
		child->SetRelativeTransform(spawnTransform);
		//Get a reference to the child's class and set it's letter
		AKCD_Letters* letterObject = Cast<AKCD_Letters>(child->GetChildActor());
		if(letterObject != nullptr)
		{
			letterObject->SetLetter(FName(letter));
		} else
		{
			UE_LOG(LogTemp, Warning, TEXT("Letter object reference NULL"));
		}
	
		LettersInstances.Add(letterObject);
		x++;
	}
}

void AKCD_Ship::Untargeted()
{
	for (auto letter : LettersInstances)
	{
		letter->Unhighlight();
	}

	CurrentLetterIndex = 0;
	CurrentLetter = LettersInstances[CurrentLetterIndex]->CurrentLetter;

	StartTime = 0;

	SubTargetSprite->SetVisibility(false);
	MainTargetSprite->SetVisibility(false);
}

bool AKCD_Ship::Hit(FName Letter)
{
	//We check if there still is letters in the list.
	//Mostly a failsafe since the ship should no longer be
	//targeted when this list is empty
	if(LettersInstances.IsEmpty())
		return false;
	
	//If the letter requested is the current targetable one, We hide the letter and
	//highlight the next one or destroy the ship if it was the last
	if(!isDestroyed)
	{
		TotalTypeWord += Letter.ToString().ToLower();

		// if(FMath::IsNearlyEqual(GetNum(TotalTypeWord), GetNum(CurrentWord), 0.5))
		// {
		// 	UE_LOG(LogTemp, Warning, TEXT("Wanted word : %s"), *CurrentWord);
		// 	UE_LOG(LogTemp, Warning, TEXT("Looked at word : %s"), *TotalTypeWord);
		// 	UE_LOG(LogTemp, Warning, TEXT("Word distance at same size : %i"), EditDistance());
		// }
		
		if (LettersInstances[CurrentLetterIndex]->CurrentLetter == Letter)
		{
			LettersInstances[CurrentLetterIndex]->Hide();

			CurrentLetterIndex++;

			if(StartTime == 0)
			{
				UE_LOG(LogTemp, Warning, TEXT("%s : Started Timer"))
				StartTime = GetWorld()->GetRealTimeSeconds();
			}

			if (LettersInstances.Num() <= CurrentLetterIndex)
			{
				ShipDestroyed();
				return true;
			}

			CurrentLetter = LettersInstances[CurrentLetterIndex]->CurrentLetter;
			if(IsPrimaryTarget)
			{
				LettersInstances[CurrentLetterIndex]->PrimaryTargetHighlight();
			} else
			{
				LettersInstances[CurrentLetterIndex]->Highlight();
				SubTargetSprite->SetVisibility(true);
				MainTargetSprite->SetVisibility(false);
			}
			return true;
		}
	}

	if(IsPrimaryTarget)
		Stats.Mistakes++;
	
	return false;
}

void AKCD_Ship::ShipDestroyed()
{
	isDestroyed = true;
	OnShipDestroyedDelegate.Broadcast(this);

	if(ShipExplosionVFX != nullptr)
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ShipExplosionVFX, this->GetTransform().GetLocation());
	
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), ShipDestroyedSound,
		this->GetTransform().GetLocation());

	const double endTime = GetWorld()->GetRealTimeSeconds();
	
	const double timeToComplete = endTime - StartTime;

	Stats.TimeTaken = timeToComplete;
	
	//TODO : Send Stats to file
	//
	// UE_LOG(LogTemp, Warning, TEXT("Wanted word : %s"), *CurrentWord);
	// UE_LOG(LogTemp, Warning, TEXT("Looked at word : %s"), *TotalTypeWord);
	// UE_LOG(LogTemp, Warning, TEXT("Word distance at completion : %i"), EditDistance());

	UE_LOG(LogTemp, Warning, TEXT("%s"), *FString::SanitizeFloat(((GetNum(CurrentWord) - 1 )/Stats.TimeTaken * 60 * 0.2)));
	
	Stats.Score = ((GetNum(CurrentWord) - 1 )/Stats.TimeTaken) * 60 * 0.2;
	
	AKCD_GameMode* gamemode = Cast<AKCD_GameMode>(UGameplayStatics::GetGameMode(this));
	
	gamemode->GetShipSpawner()->AddStats(Stats);
	
	this->Destroy();
}

void AKCD_Ship::SetShipSpeed(float Modifier)
{
	ProjectileMovement->InitialSpeed = BaseSpeed * Modifier;
}

void AKCD_Ship::SetMainTarget()
{
	IsPrimaryTarget = true;
	LettersInstances[CurrentLetterIndex]->PrimaryTargetHighlight();

	SetActorLocation(GetTransform().GetLocation() + FVector(-5.0, 0.0, 0.0));

	SubTargetSprite->SetVisibility(false);
	MainTargetSprite->SetVisibility(true);

	if(CurrentLetterIndex == 0)
		Stats.WasAltTarget = false;
}
