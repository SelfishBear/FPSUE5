// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSGame/Public/Core/FP_HUD.h"

AFP_HUD::AFP_HUD()
{
}

void AFP_HUD::BeginPlay()
{
	Super::BeginPlay();

	CreateHUDWidget();
}

