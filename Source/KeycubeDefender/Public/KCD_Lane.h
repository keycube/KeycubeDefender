// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KCD_Lane.generated.h"

UCLASS()
class KEYCUBEDEFENDER_API AKCD_Lane : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Components")
	USceneComponent* SceneComponent;
	
public:	
	// Sets default values for this actor's properties
	AKCD_Lane();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Variable")
	int NumberOfLanes = 0;
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
