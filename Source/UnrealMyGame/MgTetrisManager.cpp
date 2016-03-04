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

int AMgTetrisManager::GetTetrisIndex(int x, int y, int z)
{
	return MapSize*MapHeight * x + MapHeight * y + z;
}

FVector AMgTetrisManager::GetCubeLocation(AMgBlockCubeActor* CubeActor)
{
	return BaseLocation + FVector(X + CubeActor->GetX(), Y + CubeActor->GetY(), Z + CubeActor->GetZ()) * 110.f;
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

	X = 0; Y = 0; Z = StartHeight - 1;
	CurHeight = 0;
	BlockFallSync = BlockFallPeriod;
	FallingCubeNum = 3;
	
	const FRotator SpawnRotation = GetActorRotation();
	// temp
	auto CubeActor0 = World->SpawnActor<AMgBlockCubeActor>(AMgBlockCubeActor::StaticClass(), BaseLocation, SpawnRotation);
	auto CubeActor1 = World->SpawnActor<AMgBlockCubeActor>(AMgBlockCubeActor::StaticClass(), BaseLocation, SpawnRotation);
	auto CubeActor2 = World->SpawnActor<AMgBlockCubeActor>(AMgBlockCubeActor::StaticClass(), BaseLocation, SpawnRotation);
	CubeActor0->SetCoordinate(0, 0, 0); CubeActor0->SetActorLocation(GetCubeLocation(CubeActor0));
	CubeActor1->SetCoordinate(1, 0, 0); CubeActor1->SetActorLocation(GetCubeLocation(CubeActor1));
	CubeActor2->SetCoordinate(0, 1, 0); CubeActor2->SetActorLocation(GetCubeLocation(CubeActor2));
	FallingCubeArray[0] = CubeActor0;
	FallingCubeArray[1] = CubeActor1;
	FallingCubeArray[2] = CubeActor2;

	UE_LOG(LogTemp, Log, TEXT("CreateNewCubeBlock: End"));
}

void AMgTetrisManager::MoveBlock(int x, int y)
{
	UE_LOG(LogTemp, Log, TEXT("MoveBlock: %d, %d"), x, y);
	for (int i = 0; i < FallingCubeNum; i++)
	{
		auto Actor = FallingCubeArray[i];
		int NewX = X + Actor->GetX() + x;
		int NewY = Y + Actor->GetY() + y;
		int NewZ = Z + Actor->GetZ();
		int Index = GetTetrisIndex(NewX, NewY, NewZ);
		if ((NewX < 0 || NewX >= MapSize) || (NewY < 0 || NewY >= MapSize) || PiledCubeArray[Index] != NULL)
		{
			UE_LOG(LogTemp, Log, TEXT("MoveBlock: return for %d %d %d"), NewX, NewY, NewZ);
			return;
		}
	}

	X += x; Y += y;
	for (int i = 0; i < FallingCubeNum; i++)
	{
		auto Actor = FallingCubeArray[i];
		Actor->SetActorLocation(GetCubeLocation(Actor));
	}
}

void AMgTetrisManager::DownBlock()
{
	if (Z == 0)
	{
		UE_LOG(LogTemp, Log, TEXT("StopBlock: Height 0"));
		StopBlock();
	}
	else
	{
		for (int i = 0; i < FallingCubeNum; i++)
		{
			auto Actor = FallingCubeArray[i];
			int NewX = X + Actor->GetX();
			int NewY = Y + Actor->GetY();
			int NewZ = Z + Actor->GetZ() - 1;
			int Index = GetTetrisIndex(NewX, NewY, NewZ);
			if (NewZ < 0 || Index < 0 || PiledCubeArray[Index] != NULL)
			{
				UE_LOG(LogTemp, Log, TEXT("StopBlock: for %d %d %d / %d %p"), 
					NewX, NewY, NewZ, Index, PiledCubeArray[Index]);
				StopBlock();
				return;
			}
		}

		UE_LOG(LogTemp, Log, TEXT("DownBlock: Height %d -> %d"), Z, Z - 1);
		Z--;
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
		int NewX = X + Actor->GetX();
		int NewY = Y + Actor->GetY();
		int NewZ = Z + Actor->GetZ();
		int Index = GetTetrisIndex(NewX, NewY, NewZ);
		Actor->SetPiled();
		PiledCubeArray[Index] = Actor;
		FallingCubeArray[i] = NULL;
		UE_LOG(LogTemp, Log, TEXT("StopBlock: PileCube for %d %d %d / %d"), NewX, NewY, NewZ, Index);
	}
	FallingCubeNum = 0;
}

void AMgTetrisManager::RotateX()
{
	for (int i = 0; i < FallingCubeNum; i++)
	{
		auto Actor = FallingCubeArray[i];
		int RotX = Actor->GetX();
		int RotY = Actor->GetZ();
		int RotZ = -Actor->GetY();
		int NewX = X + RotX;
		int NewY = Y + RotY;
		int NewZ = Z + RotZ;
		int Index = GetTetrisIndex(NewX, NewY, NewZ);
		if ((NewX < 0 || NewX >= MapSize) || (NewY < 0 || NewY >= MapSize) || NewZ < 0 || PiledCubeArray[Index] != NULL)
		{
			UE_LOG(LogTemp, Log, TEXT("RotateX: Failed by %d %d %d / %d"), NewX, NewY, NewZ, Index);
			return;
		}
	}
	for (int i = 0; i < FallingCubeNum; i++)
	{
		auto Actor = FallingCubeArray[i];
		int RotX = Actor->GetX();
		int RotY = Actor->GetZ();
		int RotZ = -Actor->GetY();
		Actor->SetCoordinate(RotX, RotY, RotZ); Actor->SetActorLocation(GetCubeLocation(Actor));
	}
}

void AMgTetrisManager::RotateY()
{
}

void AMgTetrisManager::RotateZ()
{
}


