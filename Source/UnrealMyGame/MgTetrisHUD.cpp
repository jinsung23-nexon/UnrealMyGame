// Fill out your copyright notice in the Description page of Project Settings.

#include "UnrealMyGame.h"
#include "MgTetrisHUD.h"


AMgTetrisHUD::AMgTetrisHUD()
{
	ClearCubeNum = 0;

	static ConstructorHelpers::FObjectFinder<UFont> TestFontFinder(
		TEXT("/Game/Fonts/H2GTRE.H2GTRE"));
	TestFont = TestFontFinder.Object;
}

void AMgTetrisHUD::DrawHUD()
{
	Super::DrawHUD();

	FCanvasTextItem TextItem(FVector2D::ZeroVector, FText::GetEmpty(), TestFont, FLinearColor::Red);

	TextItem.Text = FText::FromString(TEXT("UnrealMyGame"));
	Canvas->DrawItem(TextItem, 10.0f, 10.0f);

	FFormatNamedArguments Args;
	Args.Add("ClearCubeNum", ClearCubeNum);
	TextItem.Text = FText::Format(NSLOCTEXT("Test", "Key", "Clear cubes: {ClearCubeNum}"), Args);
	Canvas->DrawItem(TextItem, 10.0f, 50.0f);
}
