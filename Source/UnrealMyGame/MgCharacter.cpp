// Fill out your copyright notice in the Description page of Project Settings.

#include "UnrealMyGame.h"
#include "MgCharacter.h"
#include "MgBlockCubeActor.h"


// Sets default values
AMgCharacter::AMgCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// MyGameCode
	CharStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CharStaticMesh"));
	CharStaticMesh->AttachTo(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Game/MobileStarterContent/Shapes/Shape_Cube.Shape_Cube"));
	if (MeshAsset.Succeeded())
	{
		CharStaticMesh->SetStaticMesh(MeshAsset.Object);
		CharStaticMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -40.0f));
		//CharStaticMesh->SetWorldScale3D(FVector(0.8f));
	}

	MyCameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("MyCameraSpringArm"));
	MyCameraSpringArm->AttachTo(RootComponent);
	MyCameraSpringArm->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 50.0f), FRotator(-60.0f, 0.0f, 0.0f));
	MyCameraSpringArm->TargetArmLength = 400.f;
	MyCameraSpringArm->CameraLagSpeed = 3.0f;
	MyCameraSpringArm->bEnableCameraLag = true;
	MyCameraSpringArm->bUsePawnControlRotation = true;

	MyCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("MyGameCamera"));
	MyCamera->AttachTo(MyCameraSpringArm, USpringArmComponent::SocketName);
	MyCamera->bUsePawnControlRotation = false;

}

// Called when the game starts or when spawned
void AMgCharacter::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AMgCharacter::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	// ZoomIn
	if (bZoomingIn)
	{
		ZoomFactor += DeltaTime / 0.5f;
	}
	else
	{
		ZoomFactor -= DeltaTime / 0.25f;
	}
	ZoomFactor = FMath::Clamp<float>(ZoomFactor, 0.0f, 1.0f);
	MyCamera->FieldOfView = FMath::Lerp<float>(90.0f, 60.0f, ZoomFactor);
	MyCameraSpringArm->TargetArmLength = FMath::Lerp<float>(400.0f, 300.0f, ZoomFactor);

	if (!CameraInput.IsZero())
	{
		AddControllerYawInput(CameraInput.X * DeltaTime * 40.f);
		AddControllerPitchInput(CameraInput.Y * DeltaTime * 50.f);
	}

	// Movement XY
	if (!MovementInput.IsZero())
	{
		MovementInput = MovementInput.SafeNormal() * 500.0f;
		FVector NewLocation = GetActorLocation();
		NewLocation += GetActorForwardVector() * MovementInput.X * DeltaTime;
		NewLocation += GetActorRightVector() * MovementInput.Y * DeltaTime;
		SetActorLocation(NewLocation);
	}
}

// Called to bind functionality to input
void AMgCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

	// MyGameCode : ActionMapping
	InputComponent->BindAction("ZoomIn", IE_Pressed, this, &AMgCharacter::ZoomIn);
	InputComponent->BindAction("ZoomOut", IE_Pressed, this, &AMgCharacter::ZoomOut);
	InputComponent->BindAction("Keyboard1", IE_Pressed, this, &AMgCharacter::Keyboard1);
	InputComponent->BindAction("Keyboard2", IE_Pressed, this, &AMgCharacter::Keyboard2);
	InputComponent->BindAction("Keyboard3", IE_Pressed, this, &AMgCharacter::Keyboard3);
	// MyGameCode : AxisMapping
	InputComponent->BindAxis("MoveForward", this, &AMgCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AMgCharacter::MoveRight);
	InputComponent->BindAxis("MouseX", this, &AMgCharacter::YawCamera);
	InputComponent->BindAxis("MouseY", this, &AMgCharacter::PitchCamera);
}

void AMgCharacter::Keyboard1()
{
	// Create Block Test
	const FRotator SpawnRotation = GetControlRotation();
	const FVector SpawnLocation = GetActorLocation() + SpawnRotation.RotateVector(FVector(500.f, 0.f, 100.f));
	UWorld* const World = GetWorld();
	if (World != NULL)
	{
		World->SpawnActor<AMgBlockCubeActor>(AMgBlockCubeActor::StaticClass(), SpawnLocation, SpawnRotation);
	}
}
void AMgCharacter::Keyboard2()
{
}
void AMgCharacter::Keyboard3()
{
}
