// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "HeavtObjectHUD.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "TextureResource.h"
#include "CanvasItem.h"
#include "GFTUserWidget.h"
#include "UObject/ConstructorHelpers.h"


AHeavtObjectHUD::AHeavtObjectHUD()
{
	// Set the crosshair texture
	static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshairTexObj(TEXT("/Game/FirstPerson/Textures/FirstPersonCrosshair"));
	CrosshairTex = CrosshairTexObj.Object;
}


void AHeavtObjectHUD::DrawHUD()
{
	Super::DrawHUD();

	// Draw very simple crosshair

	// find center of the Canvas
	const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

	// offset by half the texture's dimensions so that the center of the texture aligns with the center of the Canvas
	const FVector2D CrosshairDrawPosition( (Center.X),
										   (Center.Y + 20.0f));

	// draw the crosshair
	FCanvasTileItem TileItem( CrosshairDrawPosition, CrosshairTex->Resource, FLinearColor::White);
	TileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem( TileItem );
}

void AHeavtObjectHUD::BeginPlay()
{
	Super::BeginPlay();

	if (GFTWidgetClass && !GFTWidget)
	{
		GFTWidget = CreateWidget<UGFTUserWidget>(AActor::GetWorld(), GFTWidgetClass);

		if (GFTWidget)
		{
			GFTWidget->AddToViewport(0);
		}
	}
}
