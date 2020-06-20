// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/HUD.h"
#include "HeavtObjectHUD.generated.h"

UCLASS()
class AHeavtObjectHUD : public AHUD
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category="Widgets")
    TSubclassOf<UUserWidget> GFTWidgetClass;
	
	AHeavtObjectHUD();

	virtual void DrawHUD() override;
	virtual void BeginPlay() override;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

	UPROPERTY()
	class UGFTUserWidget* GFTWidget;

};

