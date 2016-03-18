// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "MgBpLibrary.generated.h"

struct FMgBlockData;

/**
 * 
 */
UCLASS()
class UNREALMYGAME_API UMgBpLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "BpLibrary")
	static int32 LoadBlockData(TArray<FMgBlockData>& BlockShapeArray);

	UFUNCTION(BlueprintCallable, Category = "BpLibrary")
	static void SortIntArray(UPARAM(ref) TArray<int32>& Array);
	
};
