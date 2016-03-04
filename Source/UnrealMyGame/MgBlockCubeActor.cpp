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
	}

	static ConstructorHelpers::FObjectFinder<UMaterial> MaterialFinder(
		TEXT("/Game/MobileStarterContent/Materials/M_Metal_Steel.M_Metal_Steel"));
	PiledMaterial = MaterialFinder.Object;
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

void AMgBlockCubeActor::SetPiled()
{
	auto DynamicMaterial = UMaterialInstanceDynamic::Create(PiledMaterial, this);
	Cube->SetMaterial(0, DynamicMaterial);
}


