#pragma once

#include "CoreMinimal.h"
#include "HeavyObject.h"
#include "Components/ActorComponent.h"
#include "GFTComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HEAVTOBJECT_API UGFTComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	float NumberOfLinks = 8;
	UPROPERTY(EditDefaultsOnly)
	//Max lift in kilograms
	float MaxLift = 1000;
	UPROPERTY(EditDefaultsOnly)
	//Distance above the ground. Within this range object will try to be.
	FVector2D VerticalRange = FVector2D(100, 200);
	UPROPERTY(EditDefaultsOnly)
	//Distance from character that object will be
	FVector2D HorizontalRange = FVector2D(100, 500);
	
	UGFTComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Interact(AHeavyObject* HeavyObject);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TArray<AHeavyObject*> CurrentHeavyObjects;

	FVector GetVerticalForceToAdd(AHeavyObject* Object) const;
	FVector GetVerticalStableForce(AHeavyObject* Object) const;

	FVector GetHorizontalForceToAdd(AHeavyObject* Object) const;
	FVector GetVerticalForceDirection(AHeavyObject* Object) const;

	float GetTotalMass();
	float GetMass(AHeavyObject* Object) const;

};
