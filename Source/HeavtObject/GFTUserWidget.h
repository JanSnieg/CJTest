// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GFTUserWidget.generated.h"

class UGFTComponent;
class UTextBlock;

UCLASS()
class HEAVTOBJECT_API UGFTUserWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* Quantity = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* Weight = nullptr;

	explicit UGFTUserWidget(const FObjectInitializer& ObjectInitializer);
	UFUNCTION()
	void UpdateCount(UGFTComponent* GFT);

	void StoreWidgetAnimations();
	UWidgetAnimation* GetAnimationByName(FName AnimationName);

protected:
	void NativeConstruct() override;
	virtual void NativeOnInitialized() override;

private:
	TMap<FName, UWidgetAnimation*> AnimationsMap;
	UWidgetAnimation* FadeAnimation;
	UWidgetAnimation* UpdateAnimation;
};
