// Fill out your copyright notice in the Description page of Project Settings.


#include "KCD_Ship.h"

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
	WordVisual = CreateDefaultSubobject<UTextRenderComponent>("WordRenderer");
	WordVisual->SetupAttachment(RootComponent);
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

	LettersLeft = UKismetStringLibrary::GetCharacterArrayFromString(CurrentWord.Word);

	WordVisual->Text = FText::AsCultureInvariant(CurrentWord.Word);
	UpdateWordVisual();
}

void AKCD_Ship::UpdateWordVisual()
{
	if(CurrentWord.Word != "")
		WordVisual->SetText(FText::AsCultureInvariant(CurrentWord.Word));
}

void AKCD_Ship::Targeted()
{
}

void AKCD_Ship::Untargeted()
{
}

bool AKCD_Ship::Hit(FString Letter)
{
	return false;
}

void AKCD_Ship::ShipDestroyed()
{
}
