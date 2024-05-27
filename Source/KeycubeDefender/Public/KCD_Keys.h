// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TextRenderComponent.h"
#include "GameFramework/Actor.h"
#include "KCD_Keys.generated.h"

UCLASS()
class KEYCUBEDEFENDER_API AKCD_Keys : public AActor
{
	GENERATED_BODY()

	//component of the key object
	UPROPERTY(VisibleAnywhere, Category=Mesh)
	UStaticMeshComponent* KeyMesh;
	UPROPERTY(VisibleAnywhere, Category=Text)
	UTextRenderComponent* KeyText;
	
public:	
	// Sets default values for this actor's properties
	AKCD_Keys();

	//Materials and color for the feedback
	UPROPERTY(EditAnywhere, Category = Variables)
	FLinearColor PressedColor;
	UPROPERTY(EditAnywhere, Category = Variables)
	FLinearColor BaseColor;
	UPROPERTY(EditAnywhere, Category = Variables)
	UMaterialInterface* BaseMaterial;

	//Input linked to the key
	UPROPERTY(EditAnywhere, Category = Variables)
	FKey AssociatedKey;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, Category = Variables)
	UMaterialInstanceDynamic* DynamicMat;

	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Visual feedback of the key press
	UFUNCTION(BlueprintCallable)
	void KeyPressed_Keys();
	//Visual feedback of the key release
	UFUNCTION(BlueprintCallable)
	void KeyReleased_Keys();

};
