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

	UPROPERTY(BlueprintAssignable, Category = Cube)
	FOnCubeSpawned CubeSpawnedDelegate;

	//If no other cube types has been specified in the options (or the selected one is invalid), this will be spawned instead
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Defaults)
	TSubclassOf<AKVA_CubeVisual> DefaultCube;
	
	UFUNCTION(BlueprintCallable, Category = Cube)
	AKVA_CubeVisual* GetCube() {return Cube;};

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
private :
	//Checks in the saved options to find the desired cube type
	UFUNCTION()
	TSubclassOf<AKVA_CubeVisual> FetchCubeType();

	//Spawns the new cube
	UFUNCTION()
	void ChangeCubeType();

	UPROPERTY()
	AKVA_CubeVisual* Cube;

};
