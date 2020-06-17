// Fill out your copyright notice in the Description page of Project Settings.


#include "HeavyObject.h"

#include "Components/StaticMeshComponent.h"

AHeavyObject::AHeavyObject()
{
	PrimaryActorTick.bCanEverTick = true;
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("Static Mesh");
	SetRootComponent(MeshComponent);
	MeshComponent->SetSimulatePhysics(true);
}

void AHeavyObject::BeginPlay()
{
	Super::BeginPlay();
}

void AHeavyObject::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHeavyObject::Interact(const bool bIsInteracting)
{
	if (MeshComponent && InteractiveMaterial)
	{
		if (bIsInteracting)
		{
			InitialMaterial = MeshComponent->GetMaterial(0);
			MeshComponent->SetMaterial(0, InteractiveMaterial);
		}
		else
		{
			MeshComponent->SetMaterial(0, InitialMaterial);
		}
	}
}

