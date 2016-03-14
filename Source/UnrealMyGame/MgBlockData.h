// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MgBlockData.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct UNREALMYGAME_API FMgBlockData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIntVector MidVec;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FIntVector> CubeVecArray;
	
};
