// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KCD_Lane.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "KCD_LaneHolder.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShipCrashed);
UCLASS()
class KEYCUBEDEFENDER_API AKCD_LaneHolder : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Components")
	USceneComponent* SceneComponent;

	UPROPERTY(EditAnywhere, Category="Components")
	UBoxComponent* HitBox;


public:	
	// Sets default values for this actor's properties
	AKCD_LaneHolder();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Variable")
	TArray<AKCD_Lane*> Lanes;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void FillLanes();
	UFUNCTION()
	void OnOverlap(AActor* MyActor, AActor* OtherActor);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Event delegate for ship hitting the city
	UPROPERTY(BlueprintAssignable)
	FOnShipCrashed OnShipCrashedDelegate;
};