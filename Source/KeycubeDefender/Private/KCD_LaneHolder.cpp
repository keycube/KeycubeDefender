// Fill out your copyright notice in the Description page of Project Settings.


#include "KCD_LaneHolder.h"

#include "KCD_Ship.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AKCD_LaneHolder::AKCD_LaneHolder()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>("LaneRoot");
	RootComponent = SceneComponent;

	HitBox = CreateDefaultSubobject<UBoxComponent>("HitZone");
	HitBox->SetupAttachment(SceneComponent);

	ProximityBox = CreateDefaultSubobject<UBoxComponent>("ProximityZone");
	ProximityBox->SetupAttachment(SceneComponent);

	VisualBar = CreateDefaultSubobject<UStaticMeshComponent>("VisualBar");
	VisualBar->SetupAttachment(SceneComponent);
}

void AKCD_LaneHolder::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	UWorld* w= GetWorld() ;

	if(w->IsValidLowLevel())
	{
		w->GetTimerManager().ClearAllTimersForObject(this);
	}
}

// Called when the game starts or when spawned
void AKCD_LaneHolder::BeginPlay()
{
	Super::BeginPlay();

	DynMaterial = UMaterialInstanceDynamic::Create(Material, this);
	VisualBar->SetMaterial(0, DynMaterial);
	SpawnLanes();

	//FillLanes();

	// Binding of the overlap function to the hitbox component
	HitBox->OnComponentBeginOverlap.AddDynamic(this, &AKCD_LaneHolder::OnBoxBeginOverlap);
	ProximityBox->OnComponentBeginOverlap.AddDynamic(this, &AKCD_LaneHolder::OnProximityBeginOverlap);
}

void AKCD_LaneHolder::SpawnLanes()
{
	
	if(NumbOfLanes <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("No lannes to be spawned"));
		return;
	}

	FTransform spawnTransform{
		this->GetTransform().GetRotation().Rotator(), // Rotation
		FVector{0, 0, MapHeight}, // Translation
		FVector{1.0f, 1.0f, 1.0f} // Scale
	};

	float LaneDistance = MapWidth / (NumbOfLanes + 1);
	
	for (int x = 0; x < NumbOfLanes; x++)
	{
		//Add the lane as a child component of the LaneHolder
		UChildActorComponent* child = NewObject<UChildActorComponent>(this);
		child->SetupAttachment(RootComponent);
		child->RegisterComponent();
		child->SetChildActorClass(LaneBlueprint);
		spawnTransform.SetLocation(FVector{0, (MapWidth/2) - (LaneDistance * (x + 1)), MapHeight});
		child->SetRelativeTransform(spawnTransform);
	
		auto LaneActor = Cast<AKCD_Lane>(child->GetChildActor());
		Lanes.Add(LaneActor);
	}	
	
}

AKCD_Ship* AKCD_LaneHolder::GetClosestShip()
{

	if(CloseShips.IsEmpty())
	{
		return nullptr;
	}
	
	float distance = 10000;
	AKCD_Ship* closestShip = nullptr;

	for (auto ship : CloseShips)
	{
		if(ship->GetTransform().GetLocation().Z < distance)
		{
			distance = ship->GetTransform().GetLocation().Z;
			closestShip = ship;
		}
	}
	
	return closestShip;
}

void AKCD_LaneHolder::UpdateVisualLine()
{
	AKCD_Ship* ClosestShip =  GetClosestShip();

	if(ClosestShip == nullptr)
	{
		
		return;
	}
	
	float currentDistance = ClosestShip->GetTransform().GetLocation().Z -
			(HitBox->GetComponentTransform().GetLocation().Z + HitBox->GetScaledBoxExtent().Z/2);

	UE_LOG(LogTemp, Warning, TEXT("Distance : %s"), *FString::SanitizeFloat(currentDistance))
	
	double pulseSpeed = UKismetMathLibrary::Lerp(0.65, 0.5, currentDistance/300);
	double glow = UKismetMathLibrary::Lerp(15, 0.5, currentDistance/300);

	UE_LOG(LogTemp, Warning, TEXT("Pulse : %s"), *FString::SanitizeFloat(pulseSpeed))
	UE_LOG(LogTemp, Warning, TEXT("glow : %s"), *FString::SanitizeFloat(glow))
	
	DynMaterial->SetScalarParameterValue("PulseSpeed", pulseSpeed);
	DynMaterial->SetScalarParameterValue("Brightness", glow);
}

// Called every frame
void AKCD_LaneHolder::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(!CloseShips.IsEmpty())
	{
		if(!VisualBar->IsVisible())
		{
			VisualBar->SetVisibility(true);
		}
		
		UpdateVisualLine();
	}
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

void AKCD_LaneHolder::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<AKCD_Ship>(OtherActor))
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ShipCrashSound, GetTransform().GetLocation());
		OnShipCrashedDelegate.Broadcast();
		CityDestroy();
	}
}

void AKCD_LaneHolder::OnProximityBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (auto ship = Cast<AKCD_Ship>(OtherActor))
	{
		CloseShips.AddUnique(ship);
		ship->OnShipDestroyedDelegate.AddDynamic(this, &AKCD_LaneHolder::ShipDestroy);
	}
}

void AKCD_LaneHolder::ShipDestroy(AKCD_Ship* DestroyedShip)
{
	DestroyedShip->OnShipDestroyedDelegate.RemoveAll(this);
	CloseShips.Remove(DestroyedShip);

	if(CloseShips.IsEmpty())
	{
		VisualBar->SetVisibility(false);
	}
}

void AKCD_LaneHolder::CityDestroy()
{
	FVector TargetLocation = this->GetTransform().GetLocation();
	TargetLocation.X = TargetLocation.X - 200.0f;
	TargetLocation.Z = TargetLocation.Z + 100.0f;
	GetWorld()->SpawnActor<AActor>(ExplosionVFXFlipbook, TargetLocation, this->GetTransform().GetRotation().Rotator());

		//TODO : INVALIDATE THE TIMERS BEFORE QUITTING
		float NumbOfExplosions = (MapWidth/2)/350;

		UE_LOG(LogTemp, Warning, TEXT("Numbers of explosions loop : %s"),  *FString::SanitizeFloat(NumbOfExplosions));

	
		for(int i = 0; i < NumbOfExplosions; i++)
		{
			FTimerHandle NewTimerHandle = FTimerHandle();
			TimerHandles.Add(NewTimerHandle);

			FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this, &AKCD_LaneHolder::SpawnExplosion, TargetLocation, i);
			
			GetWorld()->GetTimerManager().SetTimer(TimerHandles[i], TimerDelegate, (0.1 * i), false);
		}
}

void AKCD_LaneHolder::SpawnExplosion(FVector Location, int OffsetIndex)
{
	float offset = OffsetIndex * 350;
	GetWorld()->SpawnActor<AActor>(ExplosionVFXFlipbook, FVector(Location.X, (Location.Y + offset), Location.Z), FRotator(0, 0, 0));
	GetWorld()->SpawnActor<AActor>(ExplosionVFXFlipbook, FVector(Location.X, (Location.Y - offset), Location.Z), FRotator(0, 0, 0));
			
}
