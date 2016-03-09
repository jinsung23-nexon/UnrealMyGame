// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/HUD.h"
#include "MgTetrisHUD.generated.h"

/**
 * 
 */
UCLASS()
class UNREALMYGAME_API AMgTetrisHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	AMgTetrisHUD();

	virtual void DrawHUD() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ClearCubeNum;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UFont* TestFont;

public:
	int32 GetClearCubeNum() { return ClearCubeNum;  }
	void SetClearCubeNum(int32 Num) { ClearCubeNum = Num; }
	
};
