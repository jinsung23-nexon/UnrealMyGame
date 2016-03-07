// Fill out your copyright notice in the Description page of Project Settings.

#include "UnrealMyGame.h"
#include "MgTetrisManager.h"
#include "MgBlockCubeActor.h"

// Sets default values
AMgTetrisManager::AMgTetrisManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    MapSize = 5;
	StartHeight = 7;
	MapHeight = StartHeight + 2;
	BlockFallPeriod = 1.f;
	BlockFallSync = BlockFallPeriod;
	FallingCubeNum = 0;

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
	
	BaseLocation = GetActorLocation() + FVector(50.f, 50.f, 20.f);
}

// Called every frame
void AMgTetrisManager::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if (FallingCubeNum > 0)
	{
		BlockFallSync -= DeltaTime;
		if (BlockFallSync < 0)
		{
			DownBlock();
		}
	}

}

int AMgTetrisManager::GetTetrisIndex(const FIntVector& vec)
{
	return MapSize*MapHeight * vec.X + MapHeight * vec.Y + vec.Z;
}

bool AMgTetrisManager::CheckCubeValid(const FIntVector& vec)
{
	int Index = GetTetrisIndex(vec);
	return vec.X >= 0 && vec.X < MapSize && vec.Y >= 0 && vec.Y < MapSize && vec.Z >= 0
		&& Index >= 0 && PiledCubeArray[Index] == NULL;
}

FVector AMgTetrisManager::GetCubeLocation(AMgBlockCubeActor* CubeActor)
{
	FIntVector CubeCoord = CurBlockCord + CubeActor->GetCoordinate();
	return BaseLocation + FVector(CubeCoord.X, CubeCoord.Y, CubeCoord.Z) * 110.f;
}

void AMgTetrisManager::CreateNewBlock()
{
	UWorld* const World = GetWorld();
	if (World == NULL) return;

	for (auto actor : FallingCubeArray)
	{
		if (actor != NULL)
		{
			UE_LOG(LogTemp, Log, TEXT("CreateNewCubeBlock: Destroy"));
			actor->Destroy();
		}
	}

	CurBlockCord.X = 0;
	CurBlockCord.Y = 0;
	CurBlockCord.Z = StartHeight - 1;
	CurHeight = 0;
	BlockFallSync = BlockFallPeriod;
	FallingCubeNum = 3;
	
	const FRotator SpawnRotation = GetActorRotation();
	// temp
	TArray<FIntVector> CubeCoords;
	CubeCoords.Add(FIntVector(0, 0, 0));
	CubeCoords.Add(FIntVector(1, 0, 0));
	CubeCoords.Add(FIntVector(0, 1, 0));
	for (int i = 0; i < CubeCoords.Num(); i++)
	{
		auto CubeActor = World->SpawnActor<AMgBlockCubeActor>(AMgBlockCubeActor::StaticClass(), BaseLocation, SpawnRotation);
		CubeActor->SetCoordinate(CubeCoords[i]);
		CubeActor->SetActorLocation(GetCubeLocation(CubeActor));
		FallingCubeArray[i] = CubeActor;

	}
	UE_LOG(LogTemp, Log, TEXT("CreateNewCubeBlock: End"));
}

void AMgTetrisManager::MoveBlock(int x, int y)
{
	UE_LOG(LogTemp, Log, TEXT("MoveBlock: %d, %d"), x, y);
	for (int i = 0; i < FallingCubeNum; i++)
	{
		auto Actor = FallingCubeArray[i];
		auto NewCoord = CurBlockCord + Actor->GetCoordinate() + FIntVector(x, y, 0);
		if (!CheckCubeValid(NewCoord))
		{
			UE_LOG(LogTemp, Log, TEXT("MoveBlock: Not valid at %d %d %d"), NewCoord.X, NewCoord.Y, NewCoord.Z);
			return;
		}
	}

	CurBlockCord += FIntVector(x, y, 0);
	for (int i = 0; i < FallingCubeNum; i++)
	{
		auto Actor = FallingCubeArray[i];
		Actor->SetActorLocation(GetCubeLocation(Actor));
	}
}

void AMgTetrisManager::DownBlock()
{
	if (CurBlockCord.Z == 0)
	{
		UE_LOG(LogTemp, Log, TEXT("StopBlock: Height 0"));
		StopBlock();
	}
	else
	{
		for (int i = 0; i < FallingCubeNum; i++)
		{
			auto Actor = FallingCubeArray[i];
			auto NewCoord = CurBlockCord + Actor->GetCoordinate() + FIntVector(0, 0, -1);
			if (!CheckCubeValid(NewCoord))
			{
				UE_LOG(LogTemp, Log, TEXT("StopBlock: Piled at %d %d %d"), NewCoord.X, NewCoord.Y, NewCoord.Z);
				StopBlock();
				return;
			}
		}

		UE_LOG(LogTemp, Log, TEXT("DownBlock: Height %d -> %d"), CurBlockCord.Z, CurBlockCord.Z - 1);
		CurBlockCord.Z--;
		for (int i = 0; i < FallingCubeNum; i++)
		{
			auto Actor = FallingCubeArray[i];
			Actor->SetActorLocation(GetCubeLocation(Actor));
		}
		BlockFallSync = BlockFallPeriod;
	}
}

void AMgTetrisManager::StopBlock()
{
	for (int i = 0; i < FallingCubeNum; i++)
	{
		auto Actor = FallingCubeArray[i];
		auto NewCoord = CurBlockCord + Actor->GetCoordinate();
		int Index = GetTetrisIndex(NewCoord);
		Actor->SetPiled();
		PiledCubeArray[Index] = Actor;
		FallingCubeArray[i] = NULL;
		UE_LOG(LogTemp, Log, TEXT("StopBlock: PileCube for %d %d %d / %d"), NewCoord.X, NewCoord.Y, NewCoord.Z, Index);
	}
	FallingCubeNum = 0;
}

void AMgTetrisManager::RotateX()
{
	for (int i = 0; i < FallingCubeNum; i++)
	{
		auto Actor = FallingCubeArray[i];
		auto CubeCoord = Actor->GetCoordinate();
		auto NewCoord = CurBlockCord + FIntVector(CubeCoord.X, CubeCoord.Z, -CubeCoord.Y);
		int Index = GetTetrisIndex(NewCoord);
		if (!CheckCubeValid(NewCoord))
		{
			UE_LOG(LogTemp, Log, TEXT("RotateX: Failed by %d %d %d / %d"), NewCoord.X, NewCoord.Y, NewCoord.Z, Index);
			return;
		}
	}
	for (int i = 0; i < FallingCubeNum; i++)
	{
		auto Actor = FallingCubeArray[i];
		auto CubeCoord = Actor->GetCoordinate();
		Actor->SetCoordinate(FIntVector(CubeCoord.X, CubeCoord.Z, -CubeCoord.Y));
		Actor->SetActorLocation(GetCubeLocation(Actor));
	}
}

void AMgTetrisManager::RotateY()
{
}

void AMgTetrisManager::RotateZ()
{
}


