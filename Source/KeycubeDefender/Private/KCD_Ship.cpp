// Fill out your copyright notice in the Description page of Project Settings.


#include "KCD_Ship.h"
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

	LettersBackground = CreateDefaultSubobject<UPaperSpriteComponent>("LettersBackground");
	LettersBackground->SetupAttachment(RootComponent);
}

void AKCD_Ship::Initialize(int NewTier, FString NewWord, int NewWordIndex, float SpeedModifier)
{
	//Settings of values used for the ship's spawn and behavior
	Tier = NewTier;
	SetWord(NewWord);
	WordIndex = NewWordIndex;
	SetShipSpeed(SpeedModifier);
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

void AKCD_Ship::DestroyBuilding()
{
	
}

void AKCD_Ship::SetWord(FString word)
{
	CurrentWord = word;
	
	SpawnLetters();
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

	LettersBackground->SetRelativeLocation(FVector{25.0f, 0, 0.0f});
	LettersBackground->SetRelativeScale3D(FVector{0.001 * WordSize, 1.0f, 0.1f});

	//Base transform of the letters, used to spawn the letter in local position
	FTransform BackgroundTransform{
		FRotator{0.0f, -90.0f, 0.0f},                 // Rotation
		FVector{50.0f, 0, -30.0f},  // Translation
		FVector{0.001 * WordSize, 1.0f, 0.04f}   // Scale
	};

	LettersBackground->SetRelativeTransform(BackgroundTransform);

	//We spawn a KCD_Letter BP for each letters in the word 
	int x = 0;
	for (auto letter : UKismetStringLibrary::GetCharacterArrayFromString(CurrentWord))
	{
		//Position uses half the size of the word and offset that position
		//with the leght of theprevious letters
		float yPosition = (WordSize/2) - (Lettersize * (x + 0.5));
		spawnTransform.SetLocation(FVector{50.0f, yPosition, -30.0f});

		//Add the letter as a child component of the ship
		UChildActorComponent* child = NewObject<UChildActorComponent>(this);
		child->SetupAttachment(Collision);
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

	//LettersInstances[0]->Unhighlight();
}

bool AKCD_Ship::Hit(FName Letter)
{
	//We check if there still is letters in the list.
	//Mostly a failsafe since the ship should no longer be
	//targeted when this list is empty
	if(LettersInstances.IsEmpty())
		return false;

	//If the letter requested is the first non-hidden one, We hide the letter and
	//highlight the next one or destroy the ship if it was the last
	if(!isDestroyed)
	{
		for (int x = 0; x < LettersInstances.Num(); x++)
		{
			if(LettersInstances[x]->IsLetterHidden)
			{
				continue;
			}

			if(LettersInstances[x]->CurrentLetter == Letter)
			{
				LettersInstances[x]->Hide();
				
				if(LettersInstances.Last() == LettersInstances[x])
				{
					ShipDestroyed();
					return true;
				}
		
				LettersInstances[x + 1]->Highlight();
				return true;
			}
			return false;
		}
	}

	return false;
}

void AKCD_Ship::ShipDestroyed()
{
	isDestroyed = true;
	OnShipDestroyedDelegate.Broadcast(this);

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]()
	{
		this->Destroy();
	},  1, false);
}

void AKCD_Ship::SetShipSpeed(float Modifier)
{
	ProjectileMovement->InitialSpeed = BaseSpeed * Modifier;
}
