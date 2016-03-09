// Fill out your copyright notice in the Description page of Project Settings.

#include "UnrealMyGame.h"
#include "MgCharacter.h"
#include "MgBlockCubeActor.h"
#include "MgTetrisManager.h"


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

	for (auto ActorItr = TActorIterator<AActor>(GetWorld()); ActorItr; ++ActorItr)
	{
		if (ActorItr->GetName().StartsWith("MgTetrisManager"))
		{
			TetrisManager = Cast<AMgTetrisManager>(*ActorItr);
			break;
		}
	}

	TetrisInit();
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
		MovementInput = MovementInput.GetSafeNormal() * 500.0f;
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

	// MyGameCode : KeyMapping
	InputComponent->BindKey(EKeys::One, IE_Pressed, this, &AMgCharacter::TetrisInit);
	InputComponent->BindKey(EKeys::Two, IE_Pressed, this, &AMgCharacter::BlockCreate);
	InputComponent->BindKey(EKeys::Three, IE_Pressed, this, &AMgCharacter::CameraInit);
	InputComponent->BindKey(EKeys::Up, IE_Pressed, this, &AMgCharacter::BlockForward);
	InputComponent->BindKey(EKeys::Down, IE_Pressed, this, &AMgCharacter::BlockBackward);
	InputComponent->BindKey(EKeys::Left, IE_Pressed, this, &AMgCharacter::BlockLeft);
	InputComponent->BindKey(EKeys::Right, IE_Pressed, this, &AMgCharacter::BlockRight);
	InputComponent->BindKey(EKeys::W, IE_Pressed, this, &AMgCharacter::BlockForward);
	InputComponent->BindKey(EKeys::S, IE_Pressed, this, &AMgCharacter::BlockBackward);
	InputComponent->BindKey(EKeys::A, IE_Pressed, this, &AMgCharacter::BlockLeft);
	InputComponent->BindKey(EKeys::D, IE_Pressed, this, &AMgCharacter::BlockRight);
	InputComponent->BindKey(EKeys::Q, IE_Pressed, this, &AMgCharacter::CameraLeft);
	InputComponent->BindKey(EKeys::E, IE_Pressed, this, &AMgCharacter::CameraRight);
	InputComponent->BindKey(EKeys::Z, IE_Pressed, this, &AMgCharacter::BlockRotateX);
	InputComponent->BindKey(EKeys::X, IE_Pressed, this, &AMgCharacter::BlockRotateY);
	InputComponent->BindKey(EKeys::C, IE_Pressed, this, &AMgCharacter::BlockRotateZ);
	InputComponent->BindKey(EKeys::SpaceBar, IE_Pressed, this, &AMgCharacter::BlockDrop);

	// MyGameCode : ActionMapping
	InputComponent->BindAction("ZoomIn", IE_Pressed, this, &AMgCharacter::ZoomIn);
	InputComponent->BindAction("ZoomOut", IE_Pressed, this, &AMgCharacter::ZoomOut);
	// MyGameCode : AxisMapping
	InputComponent->BindAxis("MoveForward", this, &AMgCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AMgCharacter::MoveRight);
	InputComponent->BindAxis("MouseX", this, &AMgCharacter::YawCamera);
	InputComponent->BindAxis("MouseY", this, &AMgCharacter::PitchCamera);
}

void AMgCharacter::TetrisInit() { TetrisManager->InitGame(); }
void AMgCharacter::CameraInit() { TetrisManager->InitCamera(); }
void AMgCharacter::CameraLeft() { TetrisManager->RotateCamera(false); }
void AMgCharacter::CameraRight() { TetrisManager->RotateCamera(true); }
void AMgCharacter::BlockCreate() { TetrisManager->CreateBlock(); }
void AMgCharacter::BlockForward() { TetrisManager->MoveBlock(1, 0); }
void AMgCharacter::BlockBackward() { TetrisManager->MoveBlock(-1, 0); }
void AMgCharacter::BlockLeft() { TetrisManager->MoveBlock(0, -1); }
void AMgCharacter::BlockRight() { TetrisManager->MoveBlock(0, 1); }
void AMgCharacter::BlockRotateX() { TetrisManager->RotateBlock(EAxis::X); }
void AMgCharacter::BlockRotateY() { TetrisManager->RotateBlock(EAxis::Y); }
void AMgCharacter::BlockRotateZ() { TetrisManager->RotateBlock(EAxis::Z); }
void AMgCharacter::BlockDrop() { TetrisManager->DropBlock(); }
