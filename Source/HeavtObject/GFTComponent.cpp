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

FVector UGFTComponent::GetVerticalForceToAddByGround(AHeavyObject* Object) const
{
	FHitResult HitResult;
	const FVector StartLocation = Object->GetActorLocation();
	const FVector EndLocation = StartLocation + Object->GetActorUpVector() * -FLT_MAX;
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
	return StableForce;
}

FVector UGFTComponent::GetVerticalForceToAddByPlayer(AHeavyObject* Object) const
{
	const FVector StableForce = GetVerticalStableForce(Object);
	const float VerticalDistance = Object->GetActorLocation().Z - GetOwner()->GetActorLocation().Z;
	const float Multiplier = VerticalDistance / VerticalRangeByPlayer.GetMax();

	/*Distance > 0
	 *	Should go up
	 *Distance < 0
	 *	Should go down
	 *Distance == 0
	 *	Should stay in place*/
	if (VerticalDistance < VerticalRangeByPlayer.GetMin() || VerticalDistance > VerticalRangeByPlayer.GetMax())
	{
		return StableForce * (1 - Multiplier);
	}
	return StableForce;
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
	const FVector CharacterLocation = GetOwner()->GetActorLocation();
	const FVector ObjectLocation = Object->GetActorLocation();
	
	const float Distance = FVector::Dist(CharacterLocation, ObjectLocation);

	FVector VerticalDirection = (CharacterLocation - ObjectLocation) * GetMass(Object);
	VerticalDirection.Z = 0;
	
	if (Distance < HorizontalRange.GetMin())
	{
		return -VerticalDirection;
	}
	if  (Distance > HorizontalRange.GetMax())
	{
		return VerticalDirection;
	}
	return FVector(0);
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
            	PrimitiveComponent->AddForce(bUsePlayerAsComparison ? GetVerticalForceToAddByPlayer(Object) : GetVerticalForceToAddByGround(Object));
            	PrimitiveComponent->AddForce(GetHorizontalForceToAdd(Object));
				PrimitiveComponent->SetAngularDamping(AngularDamping);
				PrimitiveComponent->SetLinearDamping(LinearDamping);
            }
		}
	}
}

void UGFTComponent::Deactivation(AHeavyObject* HeavyObject, const int Index)
{
	CurrentHeavyObjects.RemoveAt(Index);
	HeavyObject->Interact(false);
	OnRemove.Broadcast(GetOwner(), HeavyObject);
}

void UGFTComponent::Activation(AHeavyObject* HeavyObject)
{
	CurrentHeavyObjects.AddUnique(HeavyObject);
	HeavyObject->Interact(true);
	OnAdd.Broadcast(GetOwner(), HeavyObject);
}

void UGFTComponent::Interact(AHeavyObject* HeavyObject)
{
	const int Index = CurrentHeavyObjects.Find(HeavyObject);
	if (Index == INDEX_NONE)
	{
		if (CurrentHeavyObjects.Num() > NumberOfLinks)
		{
			OnFail.Broadcast(GetOwner(), HeavyObject, EFailureReason::Number);
			GEngine->AddOnScreenDebugMessage(1, 5, FColor::Green,FString("Cannot carry anymore objects"));
			return;
		}
		const float MassToBe = GetTotalMass() + GetMass(HeavyObject);
		if (MassToBe > MaxLift)
		{
			OnFail.Broadcast(GetOwner(), HeavyObject, EFailureReason::OverWeight);
			GEngine->AddOnScreenDebugMessage(1, 5, FColor::Green,FString("Cannot carry anymore weight"));
			return;
		}
		Activation(HeavyObject);
	}
	else
	{
		Deactivation(HeavyObject, Index);
	}
}
