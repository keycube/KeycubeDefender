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
	UPROPERTY(EditAnywhere, Category="Word")
	UPaperSpriteComponent* LettersBackground;

	UPROPERTY(EditAnywhere, Category="Word")
	USceneComponent* LettersHolder;
	
public:	
	// Sets default values for this actor's properties
	AKCD_Ship();

	// Allows the setting of required parameters as a single function
	UFUNCTION(BlueprintCallable)
	void Initialize(int NewTier, FString NewWord, int NewWordIndex, float SpeedModifier);

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
	UPROPERTY(BlueprintAssignable)
	FOnShipDestroyed OnShipDestroyedDelegate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Words", meta = (ExposeOnSpawn=true))
	FString CurrentWord;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Words")
	int WordIndex = -1;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category="Variables")
	float BaseSpeed = 10;
	//TODO : ADD LANES
	// UPROPERTY(EditAnywhere, Category="Variables")
	// KCD_Lane* Lane;

private:
	//Theses var are used to make the word centered on the ship
	UPROPERTY(EditAnywhere, Category="Words")
	float Lettersize = 30.0;
	
	FTimerHandle TimerHandle;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	//TODO : ADD LANES
	// UFUNCTION(BlueprintCallable)
	// void SetLane(KCD_Lane lane);

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
	virtual bool Hit(FName Letter);

	//Effect when the ship has no more letters
	// broadcasts the OnSHipDestroyedDelegate
	UFUNCTION(BlueprintCallable)
	void ShipDestroyed();

	//Set the ship's speed using the wave modifier
	UFUNCTION(BlueprintCallable)
	void SetShipSpeed(float Modifier);

};
