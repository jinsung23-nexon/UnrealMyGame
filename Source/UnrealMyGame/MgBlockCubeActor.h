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
	FIntVector Coordinate;
	UStaticMeshComponent* Cube;
	UMaterial* PiledMaterial;

public:
	const FIntVector& GetCoordinate() { return Coordinate; }
	void SetCoordinate(const FIntVector& coord) { Coordinate = coord; }
	void SetPiled();

};
