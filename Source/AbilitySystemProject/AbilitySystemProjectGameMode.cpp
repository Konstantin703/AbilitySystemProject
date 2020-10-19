// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "AbilitySystemProjectGameMode.h"
#include "AbilitySystemProjectHUD.h"
#include "AbilitySystemProjectCharacter.h"
#include "UObject/ConstructorHelpers.h"

AAbilitySystemProjectGameMode::AAbilitySystemProjectGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AAbilitySystemProjectHUD::StaticClass();
}
