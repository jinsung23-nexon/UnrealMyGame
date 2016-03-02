// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "MgCharacter.generated.h"

UCLASS()
class UNREALMYGAME_API AMgCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMgCharacter();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	// MyGameCode
protected:
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* CharStaticMesh;
	UPROPERTY(EditAnywhere)
	USpringArmComponent* MyCameraSpringArm;
	UCameraComponent* MyCamera;

	FVector2D MovementInput;
	FVector2D CameraInput;
	UPROPERTY(EditAnywhere)
	float ZoomFactor;
	bool bZoomingIn;

	void ZoomIn() { bZoomingIn = true; }
	void ZoomOut() { bZoomingIn = false; }
	void MoveForward(float AxisValue) { MovementInput.X = FMath::Clamp<float>(AxisValue, -1.0f, 1.0f); }
	void MoveRight(float AxisValue) { MovementInput.Y = FMath::Clamp<float>(AxisValue, -1.0f, 1.0f); }
	void YawCamera(float AxisValue) { CameraInput.X = FMath::Clamp<float>(AxisValue, -1.0f, 1.0f); }
	void PitchCamera(float AxisValue) { CameraInput.Y = FMath::Clamp<float>(AxisValue, -1.0f, 1.0f); }
};
