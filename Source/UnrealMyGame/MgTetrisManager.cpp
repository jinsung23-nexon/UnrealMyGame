// Fill out your copyright notice in the Description page of Project Settings.

#include "UnrealMyGame.h"
#include "MgTetrisManager.h"
#include "MgBlockCubeActor.h"

// Sets default values
AMgTetrisManager::AMgTetrisManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    MapSize = 3;
    MapHeight = 5;
	BlockFallPeriod = 5.f;
	BlockFallSync = BlockFallPeriod;

    PiledCubeArray.Init(NULL, MapSize*MapSize*MapHeight);
    FallingCubeArray.Init(NULL, 3); // temp

	auto MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Floor"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Game/MobileStarterContent/Architecture/Floor_400x400.Floor_400x400"));
	if (MeshAsset.Succeeded())
	{
		MeshComp->SetStaticMesh(MeshAsset.Object);
		MeshComp->SetWorldScale3D(FVector(2.5f, 2.5f, 1.5f));
		RootComponent = MeshComp;
	}
}

// Called when the game starts or when spawned
void AMgTetrisManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMgTetrisManager::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void AMgTetrisManager::CreateNewBlock()
{
	UWorld* const World = GetWorld();
	if (World == NULL) return;

	for (auto actor : FallingCubeArray)
	{
		if (actor != NULL) actor->Destroy();
	}
	
	const FRotator SpawnRotation = GetActorRotation();
	const FVector SpawnLocation = GetActorLocation() + FVector(0.f, 0.f, MapHeight * 120.f + 10.f);
	// temp
	auto CubeActor0 = World->SpawnActor<AMgBlockCubeActor>(AMgBlockCubeActor::StaticClass(), SpawnLocation, SpawnRotation);
	auto CubeActor1 = World->SpawnActor<AMgBlockCubeActor>(AMgBlockCubeActor::StaticClass(), SpawnLocation, SpawnRotation);
	auto CubeActor2 = World->SpawnActor<AMgBlockCubeActor>(AMgBlockCubeActor::StaticClass(), SpawnLocation, SpawnRotation);
	CubeActor0->SetFalling(true, SpawnLocation); CubeActor1->SetCoordinate(0, 0, 0);
	CubeActor1->SetFalling(true, SpawnLocation); CubeActor1->SetCoordinate(1, 0, 0);
	CubeActor2->SetFalling(true, SpawnLocation); CubeActor2->SetCoordinate(0, 1, 0);
	FallingCubeArray[0] = CubeActor0;
	FallingCubeArray[1] = CubeActor1;
	FallingCubeArray[2] = CubeActor2;
	CubeNum = 3;
	UE_LOG(LogTemp, Log, TEXT("__CreateNewCubeBlock"));
}

void AMgTetrisManager::MoveBlock(int x, int y)
{
	for (int i = 0; i < CubeNum; i++)
	{
		auto Actor = FallingCubeArray[i];
		int NewX = Actor->GetX() + x, NewY = Actor->GetY() + y;
		if ((NewX < 0 || NewX >= MapSize) && (NewY < 0 || NewY >= MapSize))
		{
			return;
		}
	}
	for (int i = 0; i < CubeNum; i++)
	{
		FallingCubeArray[i]->AddCoordinate(x, y, 0);
	}
}


