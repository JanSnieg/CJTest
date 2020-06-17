// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HeavyObject.generated.h"

UCLASS()
class HEAVTOBJECT_API AHeavyObject : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditDefaultsOnly)
	UMaterialInterface* InteractiveMaterial;

	UPROPERTY(EditDefaultsOnly)
	float InteractionDistance = 300;
	
	AHeavyObject();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	void Interact(bool bIsInteracting);

private:
	UPROPERTY()
	UMaterialInterface* InitialMaterial;
};
