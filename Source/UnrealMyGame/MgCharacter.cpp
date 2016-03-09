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
	InputComponent->BindKey(EKeys::Up, IE_Pressed, this, &AMgCharacter::KeyUp);
	InputComponent->BindKey(EKeys::Down, IE_Pressed, this, &AMgCharacter::KeyDown);
	InputComponent->BindKey(EKeys::Left, IE_Pressed, this, &AMgCharacter::KeyLeft);
	InputComponent->BindKey(EKeys::Right, IE_Pressed, this, &AMgCharacter::KeyRight);
	InputComponent->BindKey(EKeys::SpaceBar, IE_Pressed, this, &AMgCharacter::KeySpace);
	InputComponent->BindKey(EKeys::Z, IE_Pressed, this, &AMgCharacter::KeyZ);
	InputComponent->BindKey(EKeys::X, IE_Pressed, this, &AMgCharacter::KeyX);
	InputComponent->BindKey(EKeys::C, IE_Pressed, this, &AMgCharacter::KeyC);
	InputComponent->BindKey(EKeys::Q, IE_Pressed, this, &AMgCharacter::KeyQ);
	InputComponent->BindKey(EKeys::E, IE_Pressed, this, &AMgCharacter::KeyE);
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

void AMgCharacter::KeyUp() { TetrisManager->MoveBlock(1, 0); }
void AMgCharacter::KeyDown() { TetrisManager->MoveBlock(-1, 0); }
void AMgCharacter::KeyLeft() { TetrisManager->MoveBlock(0, -1); }
void AMgCharacter::KeyRight() { TetrisManager->MoveBlock(0, 1); }
void AMgCharacter::KeySpace() { TetrisManager->DropBlock(); }
void AMgCharacter::KeyZ() { TetrisManager->RotateBlock(EAxis::X); }
void AMgCharacter::KeyX() { TetrisManager->RotateBlock(EAxis::Y); }
void AMgCharacter::KeyC() { TetrisManager->RotateBlock(EAxis::Z); }
void AMgCharacter::KeyQ() { TetrisManager->RotateCamera(false); }
void AMgCharacter::KeyE() { TetrisManager->RotateCamera(true); }

void AMgCharacter::Keyboard1()
{
	TetrisManager->InitGame();
}
void AMgCharacter::Keyboard2()
{
	if (TetrisManager != NULL)
	{
		TetrisManager->CreateNewBlock();
	}
}
void AMgCharacter::Keyboard3()
{
	TetrisManager->InitCamera();
}

