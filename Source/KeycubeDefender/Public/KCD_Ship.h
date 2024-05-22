// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KCD_Words.h"
#include "Components/BoxComponent.h"
#include "Components/TextRenderComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "KCD_Ship.generated.h"

UCLASS()
class KEYCUBEDEFENDER_API AKCD_Ship : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Mesh")
	UStaticMeshComponent* Mesh;
	UPROPERTY(EditAnywhere, Category="Words")
	UTextRenderComponent* WordVisual;
	UPROPERTY(EditAnywhere, Category="Collision")
	UBoxComponent* Collision;
	UPROPERTY(EditAnywhere, Category="Collision")
	UProjectileMovementComponent* ProjectileMovement;
	
public:	
	// Sets default values for this actor's properties
	AKCD_Ship();

	UPROPERTY(EditAnywhere, Category="Variables")
	int Tier;
	UPROPERTY(EditAnywhere ,Category="Variables")
	TArray<FString> LettersLeft;
	UPROPERTY(EditAnywhere, Category="Variables")
	int Reward;
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category="Variables")
	float Speed;
	//TODO : ADD LANES
	// UPROPERTY(EditAnywhere, Category="Variables")
	// KCD_Lane* Lane;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Words", meta = (ExposeOnSpawn=true))
	FKCD_Words CurrentWord;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	//TODO : ADD LANES
	// UFUNCTION(BlueprintCallable)
	// void SetLane(KCD_Lane lane);

	UFUNCTION(BlueprintCallable)
	void DestroyBuilding();

	UFUNCTION(BlueprintCallable)
	void SetWord(FKCD_Words word);

	UFUNCTION(BlueprintCallable)
	void UpdateWordVisual();

	UFUNCTION(BlueprintCallable)
	void Targeted();

	UFUNCTION(BlueprintCallable)
	void Untargeted();

	UFUNCTION(BlueprintCallable)
	bool Hit(FString Letter);

	UFUNCTION(BlueprintCallable)
	void ShipDestroyed();

};
