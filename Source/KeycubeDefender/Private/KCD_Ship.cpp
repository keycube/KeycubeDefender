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

	Collision = CreateDefaultSubobject<UBoxComponent>("BoxCollision");
	RootComponent = Collision;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("ShipMesh");
	Mesh->SetupAttachment(RootComponent);
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ShipMovement");
}

// Called when the game starts or when spawned
void AKCD_Ship::BeginPlay()
{
	Super::BeginPlay();

	UpdateWordVisual();
}

// Called every frame
void AKCD_Ship::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AKCD_Ship::DestroyBuilding()
{
	
}

void AKCD_Ship::SetWord(FKCD_Words word)
{
	CurrentWord = word;
	
	UpdateWordVisual();
	SpawnLetters();
}

void AKCD_Ship::UpdateWordVisual()
{
	//TODO : PLUG THE LETTERS LIST
}

void AKCD_Ship::Targeted()
{
	LettersInstances[0]->Highlight();
}

void AKCD_Ship::SpawnLetters()
{
	FTransform spawnTransform{
		FRotator{0.0f, -90.0f, 0.0f},                 // Rotation
		FVector{25.0f, -20.0f, 0.0f},  // Translation
		FVector{1.0f, 1.0f, 1.0f}   // Scale
	};

	WordSize = Lettersize * CurrentWord.Word.Len();
	
	int x = 0;
	for (auto letter : UKismetStringLibrary::GetCharacterArrayFromString(CurrentWord.Word))
	{
		float yPosition = (WordSize/2) - (Lettersize * (x + 0.5));
		spawnTransform.SetLocation(FVector{50.0f, yPosition, -30.0f});
	
		UChildActorComponent* child = NewObject<UChildActorComponent>(this);
		child->SetupAttachment(Collision);
		child->RegisterComponent();
		child->SetChildActorClass(LetterBP);
		child->SetRelativeTransform(spawnTransform);
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
	LettersInstances[0]->Unhighlight();
}

bool AKCD_Ship::Hit(FName Letter)
{
	if(LettersInstances.IsEmpty())
		return false;
	
	if(LettersInstances[0]->CurrentLetter == Letter && ! isDestroyed)
	{
		LettersInstances[0]->Destroy();
		LettersInstances.RemoveAt(0);

		if(LettersInstances.IsEmpty())
		{
			ShipDestroyed();
			return true;
		}
		
		//LettersInstances[1]->Highlight();
		return true;
	}

	return false;
}
//Effects on the ship during it's destruction
void AKCD_Ship::ShipDestroyed()
{
	isDestroyed = true;
	OnShipDestroyedDelegate.Broadcast(this);
}
