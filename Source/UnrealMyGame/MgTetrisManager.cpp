// Fill out your copyright notice in the Description page of Project Settings.

#include "UnrealMyGame.h"
#include "MgTetrisManager.h"
#include "MgBlockCubeActor.h"
#include "Json.h"

// Sets default values
AMgTetrisManager::AMgTetrisManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bIsInitialized = false;

	MapSize = 5;
	StartHeight = 10;
	MapHeight = StartHeight + MapSize / 2; // for Rotate Z

	MapLength = 400.f * 2.5f;
	CubeLength = MapLength / MapSize;
	CubeScale = CubeLength / 100.f;

	BlockFallPeriod = 1.f;
	CameraRotateDegree = 45.f;

	PiledCubeArray.Init(NULL, MapSize*MapHeight*MapHeight);

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

}

// Called every frame
void AMgTetrisManager::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if (BlockCubeNum > 0)
	{
		BlockFallSync -= DeltaTime;
		if (BlockFallSync <= 0)
		{
			DownBlock();
		}
	}

	if (!CameraInput.IsZero())
	{
		TetrisCamera->AddRelativeRotation(FRotator(CameraInput.Y * DeltaTime * 50.f, CameraInput.X * DeltaTime * 40.f, 0));
	}
}

void InitCubeArray(TArray<AMgBlockCubeActor*>& Array)
{
	for (int i = 0; i < Array.Num(); i++)
	{
		if (Array[i] != NULL)
		{
			Array[i]->Destroy();
			Array[i] = NULL;
		}
	}
}

void AMgTetrisManager::InitGame()
{
	if (!bIsInitialized)
	{
		/*auto Input = UGameplayStatics::GetPlayerController(this, 0)->InputComponent;
		Input->BindAxisKey(EKeys::MouseX, this, &AMgTetrisManager::YawCamera);
		Input->BindAxisKey(EKeys::MouseY, this, &AMgTetrisManager::PitchCamera);*/
		bIsInitialized = true;
	}

	BaseLocation = GetActorLocation() + FVector(100.f / 2, 100.f / 2, 100.f * 0.025f) * CubeScale;

	BlockFallSync = BlockFallPeriod;
	BlockCubeNum = 0;
	LevelCubeMax = MapSize * MapSize;
	LevelCubeNum.Init(0, MapHeight);

	InitCubeArray(PiledCubeArray);
	InitCubeArray(FallingCubeArray);
	InitCubeArray(PredictCubeArray);
	LoadBlockData();
	FallingCubeArray.Init(NULL, BlockCubeMax);
	PredictCubeArray.Init(NULL, BlockCubeMax);

	InitCamera();
	CreateBlock();
}

void AMgTetrisManager::LoadBlockData()
{
	BlockCubeMax = 0;
	BlockShpaeArray.Empty();
	FString JsonPath = FPaths::GameDir() + "/Content/Data/BlockData.json";
	FString JsonRaw = "";
	FFileHelper::LoadFileToString(JsonRaw, *JsonPath);
	TSharedPtr<FJsonObject> JsonParsed;
	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonRaw);
	if (FJsonSerializer::Deserialize(JsonReader, JsonParsed))
	{
		for (auto BlockVal : JsonParsed->GetArrayField("ShapeArray"))
		{
			auto CubeJsonArray = BlockVal->AsArray();
			TArray<FIntVector> CubeArray;
			for (auto CubeVal : CubeJsonArray)
			{
				auto VecArray = CubeVal->AsArray();
				check(VecArray.Num() == 3);
				CubeArray.Add(FIntVector(VecArray[0]->AsNumber(), VecArray[1]->AsNumber(), VecArray[2]->AsNumber()));
			}
			BlockShpaeArray.Add(CubeArray);

			if (BlockCubeMax < CubeArray.Num())
				BlockCubeMax = CubeArray.Num();
		}
	}
	check(BlockShpaeArray.Num() > 0);
}

int AMgTetrisManager::GetCubeIndex(const FIntVector& vec)
{
	return MapSize*MapHeight * vec.X + MapHeight * vec.Y + vec.Z;
}

bool AMgTetrisManager::CheckCubeValid(const FIntVector& vec)
{
	int Index = GetCubeIndex(vec);
	return vec.X >= 0 && vec.X < MapSize && vec.Y >= 0 && vec.Y < MapSize && vec.Z >= 0
		&& Index >= 0 && PiledCubeArray[Index] == NULL;
}

FVector AMgTetrisManager::GetCubeLocation(const FIntVector& vec)
{
	return BaseLocation + FVector(vec.X, vec.Y, vec.Z) * CubeLength;
}

FVector AMgTetrisManager::GetCubeLocation(AMgBlockCubeActor* CubeActor)
{
	FIntVector CubeCoord = FallingBlockCord + CubeActor->GetCoordinate();
	return GetCubeLocation(CubeCoord);
}

void AMgTetrisManager::CreateBlock()
{
	if (!bIsInitialized) return;
	UWorld* const World = GetWorld();
	if (World == NULL) return;

	InitCubeArray(FallingCubeArray);
	InitCubeArray(PredictCubeArray);

	FallingBlockCord = FIntVector(MapSize / 2, MapSize / 2, StartHeight - 1);
	PredictBlockCord = FallingBlockCord;
	BlockFallSync = BlockFallPeriod;
	
	const FRotator SpawnRotation = FRotator::ZeroRotator;
	TArray<FIntVector> CubeCoords = BlockShpaeArray[FMath::RandRange(0, BlockShpaeArray.Num()-1)];
	BlockCubeNum = CubeCoords.Num();
	for (int i = 0; i < CubeCoords.Num(); i++)
	{
		auto FallingCube = World->SpawnActor<AMgBlockCubeActor>(AMgBlockCubeActor::StaticClass(), BaseLocation, SpawnRotation);
		FallingCube->SetState(AMgBlockCubeActor::EBlockCube::BC_Falling);
		FallingCube->SetCoordinate(CubeCoords[i]);
		FallingCube->SetActorScale3D(FVector(CubeScale * 0.95f, CubeScale * 0.95f, CubeScale * 0.95f));
		FallingCube->SetActorLocation(GetCubeLocation(FallingCube));
		FallingCubeArray[i] = FallingCube;

		auto PredictCube = World->SpawnActor<AMgBlockCubeActor>(AMgBlockCubeActor::StaticClass(), BaseLocation, SpawnRotation);
		PredictCube->SetState(AMgBlockCubeActor::EBlockCube::BC_Predict);
		PredictCube->SetCoordinate(CubeCoords[i]);
		PredictCube->SetActorScale3D(FallingCube->GetActorScale3D());
		PredictCube->SetActorLocation(GetCubeLocation(PredictCube));
		PredictCubeArray[i] = PredictCube;
	}

	// rotate properly
	for (int i = 0; i < FMath::RandRange(0, 3); i++)
		RotateBlock(EAxis::Type(FMath::RandRange(1, 3)));

	PredictBlock();
	UE_LOG(LogTemp, Log, TEXT("CreateBlock: End"));
}

void AMgTetrisManager::MoveBlock(int x, int y)
{
	UE_LOG(LogTemp, Log, TEXT("MoveBlock: %d, %d"), x, y);
	for (int i = 0; i < BlockCubeNum; i++)
	{
		auto Actor = FallingCubeArray[i];
		auto AbsCoord = FallingBlockCord + Actor->GetCoordinate() + FIntVector(x, y, 0);
		if (!CheckCubeValid(AbsCoord))
		{
			UE_LOG(LogTemp, Log, TEXT("MoveBlock: Not valid at %d %d %d"), AbsCoord.X, AbsCoord.Y, AbsCoord.Z);
			return;
		}
	}

	FallingBlockCord += FIntVector(x, y, 0);
	for (int i = 0; i < BlockCubeNum; i++)
	{
		auto Actor = FallingCubeArray[i];
		Actor->SetActorLocation(GetCubeLocation(Actor));
	}

	PredictBlock();
}

void AMgTetrisManager::DownBlock()
{
	if (FallingBlockCord.Z == 0)
	{
		UE_LOG(LogTemp, Log, TEXT("StopBlock: Height 0"));
		StopBlock();
	}
	else
	{
		for (int i = 0; i < BlockCubeNum; i++)
		{
			auto Actor = FallingCubeArray[i];
			auto AbsCoord = FallingBlockCord + Actor->GetCoordinate() + FIntVector(0, 0, -1);
			if (!CheckCubeValid(AbsCoord))
			{
				UE_LOG(LogTemp, Log, TEXT("StopBlock: Piled at %d %d %d"), AbsCoord.X, AbsCoord.Y, AbsCoord.Z);
				StopBlock();
				return;
			}
		}

		UE_LOG(LogTemp, Log, TEXT("DownBlock: Height %d -> %d"), FallingBlockCord.Z, FallingBlockCord.Z - 1);
		FallingBlockCord.Z--;
		for (int i = 0; i < BlockCubeNum; i++)
		{
			auto Actor = FallingCubeArray[i];
			Actor->SetActorLocation(GetCubeLocation(Actor));
		}
		BlockFallSync = BlockFallPeriod;
	}
}

void AMgTetrisManager::StopBlock()
{
	TArray<int32> ClearLevelArray;
	for (int i = 0; i < BlockCubeNum; i++)
	{
		auto Actor = FallingCubeArray[i];
		auto AbsCoord = FallingBlockCord + Actor->GetCoordinate();
		int Index = GetCubeIndex(AbsCoord);
		Actor->SetState(AMgBlockCubeActor::EBlockCube::BC_Piled);
		PiledCubeArray[Index] = Actor;
		LevelCubeNum[AbsCoord.Z]++;
		if (LevelCubeNum[AbsCoord.Z] >= LevelCubeMax)
		{
			ClearLevelArray.Add(AbsCoord.Z);
		}

		FallingCubeArray[i] = NULL;
		UE_LOG(LogTemp, Log, TEXT("StopBlock: PileCube at %d %d %d / %d"), AbsCoord.X, AbsCoord.Y, AbsCoord.Z, Index);
	}
	BlockCubeNum = 0;

	// Clear level(floor)
	ClearLevelArray.Sort();
	int ClearCount = 0;
	if (ClearLevelArray.Num() > 0)
	{
		for (int z = ClearLevelArray[0]; z < MapHeight; z++)
		{
			if (ClearLevelArray.Num() > 0 && ClearLevelArray[0] == z)
			{
				ClearCount++;
				LevelCubeNum[z] = 0;
				for (int x = 0; x < MapSize; x++)
				{
					for (int y = 0; y < MapSize; y++)
					{
						int Index = GetCubeIndex(FIntVector(x, y, z));
						if (PiledCubeArray[Index] != NULL)
						{
							PiledCubeArray[Index]->Destroy();
							PiledCubeArray[Index] = NULL;
						}
					}
				}
				ClearLevelArray.RemoveAt(0);
				UE_LOG(LogTemp, Log, TEXT("StopBlock: ClearLevel %d"), z);
			}
			else
			{
				int MovedCubeNum = 0;
				for (int x = 0; x < MapSize; x++)
				{
					for (int y = 0; y < MapSize; y++)
					{
						int Index = GetCubeIndex(FIntVector(x, y, z));
						auto Cube = PiledCubeArray[Index];
						if (Cube != NULL)
						{
							auto NewCoord = FIntVector(x, y, z - ClearCount);
							Cube->SetActorLocation(GetCubeLocation(NewCoord));
							PiledCubeArray[GetCubeIndex(NewCoord)] = Cube;
							PiledCubeArray[Index] = NULL;
							MovedCubeNum++;
						}
					}
				}
				LevelCubeNum[z - ClearCount] = MovedCubeNum;
				UE_LOG(LogTemp, Log, TEXT("StopBlock: level %d -> %d"), z, z-ClearCount);
			}
		}
	}

	//InitCubeArray(PredictCubeArray);
	CreateBlock();
}

void AMgTetrisManager::DropBlock()
{
	FallingBlockCord = PredictBlockCord;
	for (int i = 0; i < BlockCubeNum; i++)
	{
		auto Actor = FallingCubeArray[i];
		FallingCubeArray[i]->SetActorLocation(GetCubeLocation(Actor));
	}
	StopBlock();
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
	for (int i = 0; i < BlockCubeNum; i++)
	{
		auto Actor = FallingCubeArray[i];
		auto AbsCoord = FallingBlockCord + RotateAxis(AxisType, Actor->GetCoordinate());
		int Index = GetCubeIndex(AbsCoord);
		if (!CheckCubeValid(AbsCoord))
		{
			UE_LOG(LogTemp, Log, TEXT("RotateBlock: Failed at %d %d %d / %d"), AbsCoord.X, AbsCoord.Y, AbsCoord.Z, Index);
			return;
		}
	}
	for (int i = 0; i < BlockCubeNum; i++)
	{
		auto Actor = FallingCubeArray[i];
		Actor->SetCoordinate(RotateAxis(AxisType, Actor->GetCoordinate()));
		Actor->SetActorLocation(GetCubeLocation(Actor));
	}

	PredictBlock();
}

void AMgTetrisManager::PredictBlock()
{
	if (BlockCubeNum <= 0) return;

	for (int i = 0; i < BlockCubeNum; i++)
	{
		PredictCubeArray[i]->SetCoordinate(FallingCubeArray[i]->GetCoordinate());
	}
	PredictBlockCord = FallingBlockCord;

	bool bIsBlockStop = false;
	do
	{
		PredictBlockCord.Z--;
		for (int i = 0; i < BlockCubeNum; i++)
		{
			auto Actor = PredictCubeArray[i];
			auto AbsCoord = PredictBlockCord + Actor->GetCoordinate();
			if (!CheckCubeValid(AbsCoord))
			{
				PredictBlockCord.Z++;
				bIsBlockStop = true;
				break;
			}
		}
	} while (bIsBlockStop == false);

	for (int i = 0; i < BlockCubeNum; i++)
	{
		auto Actor = PredictCubeArray[i];
		auto vec = PredictBlockCord + Actor->GetCoordinate();
		Actor->SetActorLocation(GetCubeLocation(vec));
	}
}

void AMgTetrisManager::InitCamera()
{
	TetrisCamera->SetRelativeLocationAndRotation(FVector(-200.f, 200.f, 1200.f), FRotator(-50.f, 0.f, 0.f));
	auto OurPlayerController = UGameplayStatics::GetPlayerController(this, 0);
	OurPlayerController->SetViewTargetWithBlend(this, 1.0f);
}

void AMgTetrisManager::RotateCamera(bool bIsCcw)
{
	auto CurTrans = TetrisCamera->GetRelativeTransform();
	auto CurLoc = CurTrans.GetLocation();
	auto CurRot = TetrisCamera->GetComponentRotation();
	
	float RotateDeg = bIsCcw ? -CameraRotateDegree : CameraRotateDegree;
	float RotateRad = PI / 180.f * RotateDeg;

	FVector2D MidPoint = FVector2D(200, 200);
	FVector2D XY1 = FVector2D(CurLoc.X, CurLoc.Y) - MidPoint;
	FVector2D XY2 = FVector2D(XY1.X * cos(RotateRad) - XY1.Y * sin(RotateRad),
							  XY1.X * sin(RotateRad) + XY1.Y * cos(RotateRad));
	FVector2D NewXY = MidPoint + XY2;

	TetrisCamera->SetRelativeLocation(FVector(NewXY.X, NewXY.Y, CurLoc.Z));
	TetrisCamera->SetRelativeRotation(CurRot + FRotator(0, RotateDeg, 0.f));
}