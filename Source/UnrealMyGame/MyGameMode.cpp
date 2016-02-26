// Fill out your copyright notice in the Description page of Project Settings.

#include "UnrealMyGame.h"
#include "MyGameMode.h"

AMyGameMode::AMyGameMode()
	: Super()
{
	DefaultPawnClass = NULL;
	HUDClass = NULL;
	GameStateClass = NULL;
	SpectatorClass = NULL;
}
