// Fill out your copyright notice in the Description page of Project Settings.

#include "UnrealMyGame.h"
#include "MgBpLibrary.h"
#include "MgBlockData.h"

int32 UMgBpLibrary::LoadBlockData(TArray<FMgBlockData>& BlockShapeArray)
{
	int32 BlockCubeMax = 0;
	FString JsonPath = FPaths::GameDir() + "/Content/Data/BlockData.json";
	FString JsonRaw = "";
	FFileHelper::LoadFileToString(JsonRaw, *JsonPath);
	TSharedPtr<FJsonObject> JsonParsed;
	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonRaw);
	if (FJsonSerializer::Deserialize(JsonReader, JsonParsed))
	{
		for (auto BlockVal : JsonParsed->GetArrayField("ShapeArray"))
		{
			FMgBlockData BlockData;
			auto CubeJsonArray = BlockVal->AsArray();
			for (auto CubeVal : CubeJsonArray)
			{
				auto VecArray = CubeVal->AsArray();
				check(VecArray.Num() == 3);
				BlockData.CubeVecArray.Add(FIntVector(VecArray[0]->AsNumber(), VecArray[1]->AsNumber(), VecArray[2]->AsNumber()));
			}
			BlockShapeArray.Add(BlockData);

			int32 BlockCubeNum = BlockData.CubeVecArray.Num();
			if (BlockCubeMax < BlockCubeNum)
				BlockCubeMax = BlockCubeNum;
		}
	}
	check(BlockShapeArray.Num() > 0);
	return BlockCubeMax;
}

