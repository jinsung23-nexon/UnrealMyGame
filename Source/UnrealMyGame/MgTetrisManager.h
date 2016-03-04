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
    UPROPERTY(EditAnywhere)
    int8 MapSize;
	UPROPERTY(EditAnywhere)
	int8 StartHeight;
    int8 MapHeight;
	UPROPERTY(VisibleAnywhere)
	int8 CurHeight;
	UPROPERTY(EditAnywhere)
	float BlockFallPeriod;
	float BlockFallSync;
	int8 FallingCubeNum;
	FVector BaseLocation;

	int X, Y, Z;

    TArray<AMgBlockCubeActor*> PiledCubeArray;
    TArray<AMgBlockCubeActor*> FallingCubeArray;

public:
	int GetTetrisIndex(int x, int y, int z);
	FVector GetCubeLocation(AMgBlockCubeActor* CubeActor);
	void CreateNewBlock();
	void MoveBlock(int x, int y);
	void DownBlock();
	void StopBlock();
	void RotateX();
	void RotateY();
	void RotateZ();
	
};
