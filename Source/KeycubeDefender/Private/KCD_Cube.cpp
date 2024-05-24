// Fill out your copyright notice in the Description page of Project Settings.


#include "KCD_Cube.h"

#include "AsyncTreeDifferences.h"
#include "KCD_PlayerController.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AKCD_Cube::AKCD_Cube()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CubeMesh = CreateDefaultSubobject<UStaticMeshComponent>("CubeMesh");
	RootComponent = CubeMesh;
}

// Called when the game starts or when spawned
void AKCD_Cube::BeginPlay()
{
	Super::BeginPlay();

	AKCD_PlayerController* PlayerController = Cast<AKCD_PlayerController>(UGameplayStatics::GetPlayerController(
		this, 0));

	PlayerController->KeyPressDelegate.AddDynamic(this, &AKCD_Cube::KeyPress);
	PlayerController->KeyReleaseDelegate.AddDynamic(this, &AKCD_Cube::KeyRelease);
	FillKeyMap();
}

void AKCD_Cube::FillKeyMap()
{
	TArray<USceneComponent*> ChildComponents;
	RootComponent->GetChildrenComponents(true, ChildComponents);

	UE_LOG(LogTemp, Warning, TEXT("Started map fill"));
	
	for (auto Child : ChildComponents)
	{
		if(auto ChildActor = Cast<UChildActorComponent>(Child))
		{
			if(auto ChildKey = Cast<AKCD_Keys>(ChildActor->GetChildActor()))
			{
				if(!Keys.Find(ChildKey->AssociatedKey))
				{
					Keys.Add(ChildKey->AssociatedKey, ChildKey); 
				} else
				{
					UE_LOG(LogTemp, Warning, TEXT("Key : %s Was already present"), *ChildKey->AssociatedKey.GetDisplayName().ToString());
				}
				
			}
		}
	}
}

void AKCD_Cube::KeyPress(FKey key)
{
	if(!Keys.Contains(key))
	{
		return;
	}
	
	AKCD_Keys* KeyPressed = Keys.FindRef(key);

	KeyPressed->KeyPressed_Keys();
}

void AKCD_Cube::KeyRelease(FKey key)
{
	if(!Keys.Contains(key))
	{
		return;
	}
	
	AKCD_Keys* KeyPressed = Keys.FindRef(key);

	KeyPressed->KeyReleased_Keys();
}

void AKCD_Cube::HighlightKeys(TArray<FKey> keysToHighlight)
{
	for (auto key : keysToHighlight)
	{
		
	}
}

// Called every frame
void AKCD_Cube::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AKCD_Cube::NewTarget(AKCD_Ship* ship)
{
	if(ship == nullptr)
		return;
	
	CurrentTarget = ship;

	CurrentTarget->Targeted();
}

AKCD_Ship* AKCD_Cube::GetCurrentTarget()
{
	return CurrentTarget;
}

