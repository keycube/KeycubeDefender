// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KCD_Letters.h"
#include "KCD_Words.h"
#include "Components/BoxComponent.h"
#include "Components/TextRenderComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "KCD_Ship.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShipDestroyed, AKCD_Ship*, DestroyedShip);
UCLASS()
class KEYCUBEDEFENDER_API AKCD_Ship : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Mesh")
	UStaticMeshComponent* Mesh;
	UPROPERTY(EditAnywhere, Category="Collision")
	UBoxComponent* Collision;
	UPROPERTY(EditAnywhere, Category="Collision")
	UProjectileMovementComponent* ProjectileMovement;
	
public:	
	// Sets default values for this actor's properties
	AKCD_Ship();

	UPROPERTY(EditAnywhere, Category="Variables")
	int Tier;
	// UPROPERTY(EditAnywhere ,Category="Variables")
	// TArray<FString> LettersLeft;
	UPROPERTY(EditAnywhere ,Category="Variables")
	TArray<AKCD_Letters*> LettersInstances;
	UPROPERTY(EditAnywhere ,Category="Variables")
	TSubclassOf<AKCD_Letters> LetterBP;
	UPROPERTY(EditAnywhere, Category="Variables")
	int Reward;
	UPROPERTY(EditAnywhere, Category="Variables")
	TArray<UChildActorComponent*> ChildActors;
	UPROPERTY(EditAnywhere, Category="Variables")
	bool isDestroyed = false;
	//Event delegate for ship destruction
	UPROPERTY(BlueprintAssignable, Category="Key press delegate")
	FOnShipDestroyed OnShipDestroyedDelegate;
	

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

private:
	UPROPERTY(EditAnywhere, Category="Words")
	float WordSize = 0.0;

	UPROPERTY(EditAnywhere, Category="Words")
	float Lettersize = 15.0;
	
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
	void SpawnLetters();

	UFUNCTION(BlueprintCallable)
	void Untargeted();

	UFUNCTION(BlueprintCallable)
	bool Hit(FName Letter);

	UFUNCTION(BlueprintCallable)
	void ShipDestroyed();

};
