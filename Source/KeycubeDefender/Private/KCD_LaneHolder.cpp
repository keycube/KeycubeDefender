// Fill out your copyright notice in the Description page of Project Settings.


#include "KCD_LaneHolder.h"

#include "KCD_Ship.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AKCD_LaneHolder::AKCD_LaneHolder()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>("LaneRoot");
	RootComponent = SceneComponent;

	HitBox = CreateDefaultSubobject<UBoxComponent>("HitZone");
	HitBox->SetupAttachment(SceneComponent);
}

// Called when the game starts or when spawned
void AKCD_LaneHolder::BeginPlay()
{
	Super::BeginPlay();

	SpawnLanes();

	FillLanes();

	// Bind function OnActorBeginOverlap with your class function OnOverlap
	this->OnActorBeginOverlap.AddDynamic(this, &AKCD_LaneHolder::OnOverlap);
}

void AKCD_LaneHolder::SpawnLanes()
{
	int lanesToSpawn = NumbOfLanes;

	FTransform spawnTransform{
		this->GetTransform().GetRotation().Rotator(), // Rotation
		this->GetTransform().GetLocation() + FVector{0, 0, MapHeight}, // Translation
		FVector{1.0f, 1.0f, 1.0f} // Scale
	};
	
	if(lanesToSpawn % 2 != 0)
	{
		lanesToSpawn--;
		
		//Add the letter as a child component of the ship
		UChildActorComponent* child = NewObject<UChildActorComponent>(this);
		child->SetupAttachment(RootComponent);
		child->RegisterComponent();
		child->SetChildActorClass(LaneBlueprint);
		child->SetRelativeTransform(spawnTransform);
	}
}

// Called every frame
void AKCD_LaneHolder::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AKCD_LaneHolder::FillLanes()
{
	TArray<USceneComponent*> ChildComponents;
	SceneComponent->GetChildrenComponents(true, ChildComponents);

	for (auto ChildComponent : ChildComponents)
	{
		if (auto ChildActor = Cast<UChildActorComponent>(ChildComponent))
			if (auto LaneActor = Cast<AKCD_Lane>(ChildActor->GetChildActor()))
			{
				Lanes.Add(LaneActor);
			}
	}
}

void AKCD_LaneHolder::OnOverlap(AActor* MyActor, AActor* OtherActor)
{
	if (Cast<AKCD_Ship>(OtherActor))
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ShipCrashSound, GetTransform().GetLocation());
		OnShipCrashedDelegate.Broadcast();
	}
		
}
