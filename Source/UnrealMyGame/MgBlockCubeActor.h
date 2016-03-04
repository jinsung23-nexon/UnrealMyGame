// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "MgBlockCubeActor.generated.h"

UCLASS()
class UNREALMYGAME_API AMgBlockCubeActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMgBlockCubeActor();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

protected:
	bool bIsFalling;
	int X, Y, Z;
	FVector BaseLocation;

public:
	int GetX() { return X; }
	int GetY() { return Y; }
	int GetZ() { return Z; }
	void SetFalling(bool falling, const FVector location);
	void SetCoordinate(int x, int y, int z);
	void AddCoordinate(int x, int y, int z);

};
