// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "MgCameraDirector.generated.h"

UCLASS()
class UNREALMYGAME_API AMgCameraDirector : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMgCameraDirector();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UPROPERTY(EditAnywhere)
	AActor* Camera1;

	UPROPERTY(EditAnywhere)
	AActor* Camera2;
	
	float RemainCameraChange;
};
