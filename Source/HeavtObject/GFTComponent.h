#pragma once

#include "CoreMinimal.h"
#include "HeavyObject.h"
#include "GFTComponent.generated.h"

UENUM(BlueprintType)
enum EFailureReason
{
	OverWeight,
	Number,
	Other
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HEAVTOBJECT_API UGFTComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInteract, AActor*, Owner, AHeavyObject*, Object);

	UPROPERTY(BlueprintAssignable)
	FInteract OnAdd;
	UPROPERTY(BlueprintAssignable)
	FInteract OnRemove;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FInteractFailure, AActor*, Owner, AHeavyObject*, Object, EFailureReason, Reason);
	
	UPROPERTY(BlueprintAssignable)
	FInteractFailure OnFail;
	
	UPROPERTY(EditDefaultsOnly)
	float NumberOfLinks = 8;
	UPROPERTY(EditDefaultsOnly)
	//Max lift in kilograms
	float MaxLift = 1000;

	UPROPERTY(EditDefaultsOnly)
	bool bUsePlayerAsComparison = true;
	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "!bUsePlayerAsComparison"))
	//Distance above the ground. Within this range object will try to be if we are using ground as a comparison
	FVector2D VerticalRange = FVector2D(100, 200);
	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "bUsePlayerAsComparison"))
	//Obejct will float in that distance around player's mass center.
	FVector2D VerticalRangeByPlayer = FVector2D(0, 200);
	UPROPERTY(EditDefaultsOnly)
	//Distance from character that object will be
	FVector2D HorizontalRange = FVector2D(0, 500);

	UPROPERTY(EditDefaultsOnly)
	float AngularDamping = 2;

	UPROPERTY(EditDefaultsOnly)
	float LinearDamping = 2;
	
	UGFTComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void Deactivation(AHeavyObject* HeavyObject, int Index);
	void Activation(AHeavyObject* HeavyObject);

	void Interact(AHeavyObject* HeavyObject);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TArray<AHeavyObject*> CurrentHeavyObjects;

	FVector GetVerticalForceToAddByGround(AHeavyObject* Object) const;
	FVector GetVerticalForceToAddByPlayer(AHeavyObject* Object) const;
	FVector GetVerticalStableForce(AHeavyObject* Object) const;

	FVector GetHorizontalForceToAdd(AHeavyObject* Object) const;

	float GetTotalMass();
	float GetMass(AHeavyObject* Object) const;

};
