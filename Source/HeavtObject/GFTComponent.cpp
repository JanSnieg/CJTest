#include "GFTComponent.h"


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
	const FVector EndLocation = StartLocation + Object->GetActorUpVector() * -1000;
	GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility);

	FVector ForceToAdd = GetVerticalStableForce(Object);

	if (HitResult.Distance < VerticalRange.GetMin())
	{
		ForceToAdd *= 1.1;
	}
	else if (HitResult.Distance > VerticalRange.GetMax())
	{
		ForceToAdd *= 0.9;
	}
	
	return ForceToAdd;
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

		const FVector VerticalDirection = (CharacterLocation - ObjectLocation).GetSafeNormal2D(1) * Distance;

		DrawDebugLine(GetWorld(), ObjectLocation, ObjectLocation + VerticalDirection, FColor::Blue, false, 2, 0, 2);
		
		if (Distance < HorizontalRange.GetMin())
		{
			return VerticalDirection;
		}
		if  (Distance > HorizontalRange.GetMax())
		{
			return -VerticalDirection;
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

void UGFTComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	for (AHeavyObject* Object : CurrentHeavyObjects)
	{
		if (UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(Object->GetRootComponent()))
		{
			PrimitiveComponent->AddForce(GetVerticalForceToAdd(Object));
			PrimitiveComponent->AddForce(GetHorizontalForceToAdd(Object));
		}
	}
}

void UGFTComponent::AddObject(AHeavyObject* HeavyObject)
{
	if (CurrentHeavyObjects.Num() <= NumberOfLinks)
	{
		CurrentHeavyObjects.AddUnique(HeavyObject);
	}
	else if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(0, 5, FColor::Green,FString("Reached max number of objects to carry"));
	}
}
