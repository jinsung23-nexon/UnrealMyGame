// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "MgBlockCubeActor.generated.h"

UENUM(BlueprintType)
enum class EBlockCube : uint8
{
	BC_Falling,
	BC_Predict,
	BC_Piled
};

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EBlockCube State;
	FIntVector Coordinate;
	UStaticMeshComponent* Cube;

	UPROPERTY(EditAnywhere, Category = "Material")
	UMaterial* FallingMaterial;
	UPROPERTY(EditAnywhere, Category = "Material")
	UMaterial* PredictMaterial;
	UPROPERTY(EditAnywhere, Category = "Material")
	UMaterial* PiledMaterial;

public:
	EBlockCube GetState() { return State; }
	const FIntVector& GetCoordinate() { return Coordinate; }
	void SetState(EBlockCube NewState);
	void SetCoordinate(const FIntVector& coord) { Coordinate = coord; }

};
