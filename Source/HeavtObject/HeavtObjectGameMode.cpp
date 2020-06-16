// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "HeavtObjectGameMode.h"
#include "HeavtObjectHUD.h"
#include "HeavyObjectCharacter.h"
#include "UObject/ConstructorHelpers.h"

AHeavtObjectGameMode::AHeavtObjectGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/ThisCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AHeavtObjectHUD::StaticClass();
}
