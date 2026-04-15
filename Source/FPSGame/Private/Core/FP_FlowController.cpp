// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/FP_FlowController.h"

#include "Blueprint/UserWidget.h"
#include "Core/FP_GameMode.h"
#include "Spawner/FP_WaveController.h"

void UFP_FlowController::Initialize(AFP_GameMode* InGameMode)
{
	GameMode = InGameMode;
	if (!IsValid(GameMode)) return;
	
	GameMode->OnGameStarted.AddDynamic(this, &UFP_FlowController::StartGame);
}

void UFP_FlowController::StartGame()
{
	if (!bCanSkipCountdown) return;
	if (!IsValid(GameMode->GetWaveController())) return;
	GameMode->GetWaveController()->StartNextWave();
	bCanSkipCountdown = false;
	OnCountdownFinished.Broadcast();
}

void UFP_FlowController::StartCountdown()
{
	GetWorld()->GetTimerManager().ClearTimer(CountdownTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(CountdownTickTimerHandle);
	
	OnCountdownStarted.Broadcast();
	bCanSkipCountdown = true;
	
	GetWorld()->GetTimerManager().SetTimer(CountdownTimerHandle, this, &UFP_FlowController::StartGame, CountdownData.CountdownTime, false);
	
	GetWorld()->GetTimerManager().SetTimer(CountdownTickTimerHandle, this, &UFP_FlowController::UpdateCountdownWidget, 1.f, true);
}

void UFP_FlowController::UpdateCountdownWidget()
{
	float RemainingTime = GetWorld()->GetTimerManager().GetTimerRemaining(CountdownTimerHandle);
	OnCountdownTick.Broadcast(RemainingTime);
}
