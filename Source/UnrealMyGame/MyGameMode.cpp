// Fill out your copyright notice in the Description page of Project Settings.

#include "UnrealMyGame.h"
#include "MyGameMode.h"
#include "MgCharacter.h"

AMyGameMode::AMyGameMode()
	: Super()
{
	DefaultPawnClass = AMgCharacter::StaticClass();
	HUDClass = NULL;
	GameStateClass = NULL;
	SpectatorClass = NULL;
}
