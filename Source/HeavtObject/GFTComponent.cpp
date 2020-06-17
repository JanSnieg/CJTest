#include "GFTComponent.h"

#include <FreeType2/FreeType2-2.10.0/include/freetype/config/ftstdlib.h>

#include "DrawDebugHelpers.h"
#include "HeavyObjectCharacter.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"

UGFTComponent::UGFTComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UGFTComponent::BeginPlay()
{
	Super::BeginPlay();
}

FVector UGFTComponent::GetVerticalForceToAdd(AHeavyObject* Object) const
{
	FHitResult HitResult;
	const FVector StartLocation = Object->GetActorLocation();
	const FVector EndLocation = StartLocation + Object->GetActorUpVector() * -INTMAX_MAX;
	GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility);

	const FVector StableForce = GetVerticalStableForce(Object);
	const float Multiplier = HitResult.Distance / 100;

	if (HitResult.Distance > VerticalRange.GetMax())
	{
		return StableForce * (1 - Multiplier);
	}
	if (HitResult.Distance == 0)
	{
		return StableForce * 1.2;
	}
	if (HitResult.Distance < VerticalRange.GetMin())
	{
		return StableForce * (1 + Multiplier);
	}
}

FVector UGFTComponent::GetVerticalStableForce(AHeavyObject* Object) const
{
	if (UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(Object->GetRootComponent()))
	{
		if (PrimitiveComponent->IsSimulatingPhysics())
		{
			return  FVector(0, 0, -PrimitiveComponent->GetMass() * GetWorld()->GetGravityZ());
		}
		UE_LOG(LogTemp, Warning, TEXT("Root Component of %s is not simulating physics"), *Object->GetHumanReadableName());
	}
	return FVector();
}

FVector UGFTComponent::GetHorizontalForceToAdd(AHeavyObject* Object) const
{
	if (AHeavyObjectCharacter* Character = Cast<AHeavyObjectCharacter>(GetOwner()))
	{
		const FVector CharacterLocation = Character->GetActorLocation();
		const FVector ObjectLocation = Object->GetActorLocation();
		
		const float Distance = FVector::Dist(CharacterLocation, ObjectLocation);

		FVector VerticalDirection = (CharacterLocation - ObjectLocation) * Distance * 0.1;
		VerticalDirection.Z = 0;
		
		if (Distance < HorizontalRange.GetMin())
		{
			return -VerticalDirection;
		}
		else if  (Distance > HorizontalRange.GetMax())
		{
			return VerticalDirection;
		}
	}
	return {};
}

FVector UGFTComponent::GetVerticalForceDirection(AHeavyObject* Object) const
{
	if (AHeavyObjectCharacter* Character = Cast<AHeavyObjectCharacter>(GetOwner()))
	{
		const FVector ObjectLocation = Object->GetActorLocation();
		const FVector CharacterLocation = Character->GetActorLocation();
		return (ObjectLocation - CharacterLocation).GetSafeNormal(1);
	}
	return {};
}

float UGFTComponent::GetTotalMass()
{
	float Result = 0;
	for (AHeavyObject* Object : CurrentHeavyObjects)
	{
		Result += GetMass(Object);
	}
	return Result;
}

float UGFTComponent::GetMass(AHeavyObject* Object) const
{
	if (UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(Object->GetRootComponent()))
	{
		return PrimitiveComponent->GetMass();
	}
	return 0;
}

void UGFTComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	for (AHeavyObject* Object : CurrentHeavyObjects)
	{
		if (Object)
		{
			if (UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(Object->GetRootComponent()))
            {
            	PrimitiveComponent->AddForce(GetVerticalForceToAdd(Object));
            	PrimitiveComponent->AddForce(GetHorizontalForceToAdd(Object));
            }
		}
	}
}

void UGFTComponent::Interact(AHeavyObject* HeavyObject)
{
	const int Index = CurrentHeavyObjects.Find(HeavyObject);
	if (Index == INDEX_NONE)
	{
		const float MassToBe = GetTotalMass() + GetMass(HeavyObject);
		if (CurrentHeavyObjects.Num() < NumberOfLinks && MassToBe <= MaxLift)
		{
			CurrentHeavyObjects.AddUnique(HeavyObject);
			HeavyObject->Interact(true);
		}
		else if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(1, 5, FColor::Green,FString("Cannot carry anymore objects"));
		}
	}
	else
	{
		CurrentHeavyObjects.RemoveAt(Index);
		HeavyObject->Interact(false);
	}
}
