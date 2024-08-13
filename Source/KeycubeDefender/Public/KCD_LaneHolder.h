// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KCD_Lane.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "PaperFlipbookComponent.h"
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

	UPROPERTY(EditAnywhere, Category="Components")
	UBoxComponent* ProximityBox;

	UPROPERTY(EditAnywhere, Category="Components")
	UStaticMeshComponent* VisualBar;


public:	
	// Sets default values for this actor's properties
	AKCD_LaneHolder();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Variable")
	TArray<AKCD_Lane*> Lanes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Variable")
	TSubclassOf<AKCD_Lane> LaneBlueprint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Variable")
	UNiagaraSystem* ExplosionVFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Variable")
	TSubclassOf<AActor> ExplosionVFXFlipbook;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Variable")
	int NumbOfLanes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Variable")
	float MapWidth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Variable")
	float MapHeight;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* Material;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category="Sound")
	USoundBase* ShipCrashSound;

	UPROPERTY()
	TArray<FTimerHandle> TimerHandles;

	UPROPERTY(EditAnywhere)
	AKCD_WaveManager* WaveManager;

	UPROPERTY()
	TArray<AKCD_Ship*> CloseShips;

	UPROPERTY()
	UMaterialInstanceDynamic* DynMaterial;
	
	//Fills the list of lanes
	UFUNCTION()
	void FillLanes();

	//City overlap function
	UFUNCTION()
	void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	//Proximity overlap function
	UFUNCTION()
	void OnProximityBeginOverlap(UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	//Removes the ship from the proximity list
	UFUNCTION()
	void ShipDestroy(AKCD_Ship* DestroyedShip);

	//Feedback of the city's destruction
	UFUNCTION()
	void CityDestroy();

	//Spawns explosions
	UFUNCTION()
	void SpawnExplosion(FVector Location, int OffsetIndex);

	//Spawns the lanes according to the map width and height specified
	UFUNCTION()
	void SpawnLanes();

	//Spawns the lanes according to the map width and height specified
	UFUNCTION()
	AKCD_Ship* GetClosestShip();

	//Spawns the lanes according to the map width and height specified
	UFUNCTION()
	void UpdateVisualLine();
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Event delegate for ship hitting the city
	UPROPERTY(BlueprintAssignable)
	FOnShipCrashed OnShipCrashedDelegate;
};
