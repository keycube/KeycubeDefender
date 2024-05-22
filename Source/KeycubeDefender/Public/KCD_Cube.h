// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KCD_Keys.h"
#include "GameFramework/Actor.h"
#include "KCD_Cube.generated.h"

UCLASS()
class KEYCUBEDEFENDER_API AKCD_Cube : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category=Mesh)
	UStaticMeshComponent* CubeMesh;
	
public:	
	// Sets default values for this actor's properties
	AKCD_Cube();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category=Keys)
	TMap<FKey, AKCD_Keys*> Keys;

private:
	UFUNCTION()
	void FillKeyMap();
	
	UFUNCTION(BlueprintCallable)
	void KeyPress(FKey key);

	UFUNCTION(BlueprintCallable)
	void KeyRelease(FKey key);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};