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

	MapLength = 400.f * 2.5f;
	CubeLength = MapLength / MapSize;
	CubeScale = CubeLength / 100.f;

	BlockFallPeriod = 1.f;
	BlockFallSync = BlockFallPeriod;
	CameraRotateDegree = 45.f;
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

	TetrisCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	TetrisCamera->AttachTo(RootComponent);
}

// Called when the game starts or when spawned
void AMgTetrisManager::BeginPlay()
{
	Super::BeginPlay();
	
	BaseLocation = GetActorLocation() + FVector(100.f/2, 100.f/2, 100.f * 0.025f) * CubeScale;
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
	return BaseLocation + FVector(CubeCoord.X, CubeCoord.Y, CubeCoord.Z) * CubeLength;
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
		CubeActor->SetActorScale3D(FVector(CubeScale * 0.95f, CubeScale * 0.95f, CubeScale * 0.95f));
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
		UE_LOG(LogTemp, Log, TEXT("StopBlock: PileCube at %d %d %d / %d"), NewCoord.X, NewCoord.Y, NewCoord.Z, Index);
	}
	FallingCubeNum = 0;
}

FIntVector RotateAxis(EAxis::Type AxisType, const FIntVector& vec)
{
	switch (AxisType)
	{
	case EAxis::Type::X:
		return FIntVector(vec.X, vec.Z, -vec.Y);
	case EAxis::Type::Y:
		return FIntVector(vec.Z, vec.Y, -vec.X);
	case EAxis::Type::Z:
		return FIntVector(-vec.Y, vec.X, vec.Z);
	default:
		check(false);
		return FIntVector(0, 0, 0);
	}
}

void AMgTetrisManager::RotateBlock(EAxis::Type AxisType)
{
	for (int i = 0; i < FallingCubeNum; i++)
	{
		auto Actor = FallingCubeArray[i];
		auto NewCoord = CurBlockCord + RotateAxis(AxisType, Actor->GetCoordinate());
		int Index = GetTetrisIndex(NewCoord);
		if (!CheckCubeValid(NewCoord))
		{
			UE_LOG(LogTemp, Log, TEXT("RotateBlock: Failed at %d %d %d / %d"), NewCoord.X, NewCoord.Y, NewCoord.Z, Index);
			return;
		}
	}
	for (int i = 0; i < FallingCubeNum; i++)
	{
		auto Actor = FallingCubeArray[i];
		Actor->SetCoordinate(RotateAxis(AxisType, Actor->GetCoordinate()));
		Actor->SetActorLocation(GetCubeLocation(Actor));
	}
}

void AMgTetrisManager::InitCamera()
{
	TetrisCamera->SetRelativeLocationAndRotation(FVector(-300.f, 200.f, 1000.f), FRotator(-50.f, 0.f, 0.f));
	auto OurPlayerController = UGameplayStatics::GetPlayerController(this, 0);
	OurPlayerController->SetViewTargetWithBlend(this, 0.5f);
}

void AMgTetrisManager::RotateCamera(bool bIsCcw)
{
	auto CurTrans = TetrisCamera->GetRelativeTransform();
	auto CurLoc = CurTrans.GetLocation();
	auto CurRot = TetrisCamera->GetComponentRotation();
	
	float RotateRad = PI / 180.f * CameraRotateDegree;
	FVector2D MidPoint = FVector2D(200, 200);
	FVector2D XY1 = FVector2D(CurLoc.X, CurLoc.Y) - MidPoint;
	FVector2D XY2 = FVector2D(XY1.X * cos(RotateRad) - XY1.Y * sin(RotateRad),
							  XY1.X * sin(RotateRad) + XY1.Y * cos(RotateRad));
	FVector2D NewXY = MidPoint + XY2;

	TetrisCamera->SetRelativeLocation(FVector(NewXY.X, NewXY.Y, CurLoc.Z));
	TetrisCamera->SetRelativeRotation(CurRot + FRotator(0, CameraRotateDegree, 0.f));
}