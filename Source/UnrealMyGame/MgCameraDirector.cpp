// Fill out your copyright notice in the Description page of Project Settings.

#include "UnrealMyGame.h"
#include "MgCameraDirector.h"

const float CameraChangeTime = 2.0f;
const float SmoothBlendTime = 0.75f;

// Sets default values
AMgCameraDirector::AMgCameraDirector()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMgCameraDirector::BeginPlay()
{
	Super::BeginPlay();
	
	RemainCameraChange = CameraChangeTime;
}

// Called every frame
void AMgCameraDirector::Tick(float DeltaTime)
{
	Super::Tick( DeltaTime );

	RemainCameraChange -= DeltaTime;
	if (RemainCameraChange < 0)
	{
		auto OurPlayerController = UGameplayStatics::GetPlayerController(this, 0);
		if (OurPlayerController)
		{
			if ((OurPlayerController->GetViewTarget() != Camera1) && (Camera1 != nullptr))
			{
				//Cut instantly to camera one.
				OurPlayerController->SetViewTargetWithBlend(Camera1, SmoothBlendTime);
			}
			else if ((OurPlayerController->GetViewTarget() != Camera2) && (Camera2 != nullptr))
			{
				//Blend smoothly to camera two.
				OurPlayerController->SetViewTargetWithBlend(Camera2, SmoothBlendTime);
			}
		}

		RemainCameraChange = CameraChangeTime;
	}
}

