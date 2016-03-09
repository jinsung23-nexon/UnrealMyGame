// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "MgTetrisManager.generated.h"

class AMgBlockCubeActor;

UCLASS()
class UNREALMYGAME_API AMgTetrisManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMgTetrisManager();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

protected:
	bool bIsInitialized;

    UPROPERTY(EditAnywhere)
    int8 MapSize;
	UPROPERTY(EditAnywhere)
	int8 StartHeight;
    int8 MapHeight;

	float MapLength;
	float CubeLength;
	float CubeScale;

	UPROPERTY(EditAnywhere)
	float BlockFallPeriod;
	float BlockFallSync;
	UPROPERTY(EditAnywhere)
	float CameraRotateDegree;
	int8 BlockCubeMax;
	int8 BlockCubeNum;
	int8 LevelCubeMax;
	TArray<int8> LevelCubeNum;

	FVector BaseLocation;
	FIntVector FallingBlockCord;
	FIntVector PredictBlockCord;
	FVector2D CameraInput;

	TArray<TArray<FIntVector>> BlockShpaeArray;
    TArray<AMgBlockCubeActor*> PiledCubeArray;
    TArray<AMgBlockCubeActor*> FallingCubeArray;
	TArray<AMgBlockCubeActor*> PredictCubeArray;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* TetrisCamera;

public:
	void LoadBlockData();
	void InitGame();
	int GetCubeIndex(const FIntVector& vec);
	bool CheckCubeValid(const FIntVector& vec);
	FVector GetCubeLocation(const FIntVector& vec);
	FVector GetCubeLocation(AMgBlockCubeActor* CubeActor);
	void CreateBlock();
	void MoveBlock(const FIntVector& MoveVec, bool ResetSync);
	void DownBlock();
	void StopBlock();
	void DropBlock();
	void RotateBlock(EAxis::Type AxisType);
	void PredictBlock();

	void InitCamera();
	void RotateCamera(bool bIsCcw);

	void YawCamera(float AxisValue) { CameraInput.X = FMath::Clamp<float>(AxisValue, -1.0f, 1.0f); }
	void PitchCamera(float AxisValue) { CameraInput.Y = FMath::Clamp<float>(AxisValue, -1.0f, 1.0f); }
	
};
