// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KVA_CubeVisual.h"
#include "GameFramework/Actor.h"
#include "KVA_CubeSpawner.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCubeSpawned);
UCLASS()
class KVA_API AKVA_CubeSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AKVA_CubeSpawner();

	UPROPERTY(BlueprintAssignable)
	FOnCubeSpawned CubeSpawnedDelegate;

	UFUNCTION(BlueprintCallable)
	AKVA_CubeVisual* GetCube() {return Cube;};

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
private :
	UFUNCTION()
	TSubclassOf<AKVA_CubeVisual> FetchCubeType();

	UFUNCTION()
	void ChangeCubeType();

	UPROPERTY()
	AKVA_CubeVisual* Cube;

};
