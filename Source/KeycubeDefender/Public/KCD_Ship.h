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

	//component of the ship object
	UPROPERTY(EditAnywhere, Category="Mesh")
	UStaticMeshComponent* Mesh;
	UPROPERTY(EditAnywhere, Category="Collision")
	UBoxComponent* Collision;
	UPROPERTY(EditAnywhere, Category="Collision")
	UProjectileMovementComponent* ProjectileMovement;
	
public:	
	// Sets default values for this actor's properties
	AKCD_Ship();

	//Var for the state of the ship
	UPROPERTY(EditAnywhere, Category="Variables")
	int Tier;
	UPROPERTY(EditAnywhere, Category="Variables")
	TArray<UChildActorComponent*> ChildActors;
	UPROPERTY(EditAnywhere, Category="Variables")
	bool isDestroyed = false;
	UPROPERTY(EditAnywhere, Category="Variables")
	int Reward;

	//Var for the letters to spawn
	UPROPERTY(EditAnywhere ,Category="Variables")
	TArray<AKCD_Letters*> LettersInstances;
	UPROPERTY(EditAnywhere ,Category="Variables")
	TSubclassOf<AKCD_Letters> LetterBP;

	
	//Event delegate for ship destruction
	UPROPERTY(BlueprintAssignable, Category="Key press delegate")
	FOnShipDestroyed OnShipDestroyedDelegate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Words", meta = (ExposeOnSpawn=true))
	FString CurrentWord;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Words")
	int WordIndex = -1;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category="Variables")
	float Speed = 10;
	//TODO : ADD LANES
	// UPROPERTY(EditAnywhere, Category="Variables")
	// KCD_Lane* Lane;

private:
	//Theses var are used to make the word centered on the ship
	UPROPERTY(EditAnywhere, Category="Words")
	float Lettersize = 15.0;

	FTimerHandle TimerHandle;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	//TODO : ADD LANES
	// UFUNCTION(BlueprintCallable)
	// void SetLane(KCD_Lane lane);

	//Destroy the building in the lane (or in another lane if
	//the current one is destroyed)
	UFUNCTION(BlueprintCallable)
	void DestroyBuilding();

	//Set the word associated with the ship
	UFUNCTION(BlueprintCallable)
	void SetWord(FString word);

	//Effect when the ship is targeted
	UFUNCTION(BlueprintCallable)
	void Targeted();

	//Spawn the letters for the associated word
	UFUNCTION(BlueprintCallable)
	void SpawnLetters();

	//Effect when the ship is untargeted
	UFUNCTION(BlueprintCallable)
	void Untargeted();

	//Tries to hit the ship with the letter. Returns if the hit was successful
	UFUNCTION(BlueprintCallable)
	bool Hit(FName Letter);

	//Effect when the ship has no more letters
	// broadcasts the OnSHipDestroyedDelegate
	UFUNCTION(BlueprintCallable)
	void ShipDestroyed();

};