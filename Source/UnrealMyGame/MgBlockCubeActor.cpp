// Fill out your copyright notice in the Description page of Project Settings.

#include "UnrealMyGame.h"
#include "MgBlockCubeActor.h"


// Sets default values
AMgBlockCubeActor::AMgBlockCubeActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Cube = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cube"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Game/MobileStarterContent/Shapes/Shape_Cube.Shape_Cube"));
	if (MeshAsset.Succeeded())
	{
		Cube->SetStaticMesh(MeshAsset.Object);
		Cube->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		Cube->AttachTo(RootComponent);
		RootComponent = Cube;
	}

	static ConstructorHelpers::FObjectFinder<UMaterial> FallingMatFinder(
		TEXT("/Game/MobileStarterContent/Materials/M_Basic_Wall.M_Basic_Wall"));
	FallingMaterial = FallingMatFinder.Object;
	static ConstructorHelpers::FObjectFinder<UMaterial> PredictMatFinder(
		TEXT("/Game/MobileStarterContent/Materials/M_Glass.M_Glass"));
	PredictMaterial = PredictMatFinder.Object;
	static ConstructorHelpers::FObjectFinder<UMaterial> PiledMatFinder(
		TEXT("/Game/MobileStarterContent/Materials/M_Rock_Slate.M_Rock_Slate"));
	PiledMaterial = PiledMatFinder.Object;
}

// Called when the game starts or when spawned
void AMgBlockCubeActor::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AMgBlockCubeActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void AMgBlockCubeActor::SetState(EBlockCube NewState)
{
	State = NewState;
	UMaterial* NewMaterial = NULL;
	switch (State)
	{
	case EBlockCube::BC_Falling:
		NewMaterial = FallingMaterial;
		break;
	case EBlockCube::BC_Predict:
		NewMaterial = PredictMaterial;
		break;
	case EBlockCube::BC_Piled:
		NewMaterial = PiledMaterial;
		break;
	default:
		check(false);
		break;
	}

	// SetMaterial with C++ 
	//auto DynamicMaterial = UMaterialInstanceDynamic::Create(NewMaterial, this);
	//Cube->SetMaterial(0, DynamicMaterial);

	// SetMaterial with Blueprint
	const FString Command = FString::Printf(TEXT("SetCubeMaterial %d"), (uint8)NewState);
	FOutputDeviceDebug debug;
	CallFunctionByNameWithArguments(*Command, debug, this, true);
}

void AMgBlockCubeActor::SetMaterialPiled()
{
	auto DynamicMaterial = UMaterialInstanceDynamic::Create(PiledMaterial, this);
	Cube->SetMaterial(0, DynamicMaterial);
}

