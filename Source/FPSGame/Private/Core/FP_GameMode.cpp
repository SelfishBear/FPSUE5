// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSGame/Public/Core/FP_GameMode.h"

#include "Core/FP_FlowController.h"
#include "FPSGame/Public/Character/FP_PlayerCharacter.h"
#include "FPSGame/Public/Core/FP_HUD.h"
#include "FPSGame/Public/Core/FP_PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Spawner/FP_WaveController.h"
#include "Spawner/FP_ZombieSpawner.h"

AFP_GameMode::AFP_GameMode()
{
	DefaultPawnClass = AFP_PlayerCharacter::StaticClass();
	PlayerControllerClass = AFP_PlayerController::StaticClass();
	HUDClass = AFP_HUD::StaticClass();
}

void AFP_GameMode::BeginPlay()
{
	Super::BeginPlay();
	
	SubscribeToPlayerFlowControl();
}

void AFP_GameMode::InitWaveController()
{
	WaveController = NewObject<UFP_WaveController>(this, UFP_WaveController::StaticClass());
	WaveController->OnWaveCompleted.AddDynamic(this, &AFP_GameMode::OnWaveCompleted);
	WaveController->Initialize(ZombieSpawner);
}

void AFP_GameMode::InitZombieSpawner()
{
	ZombieSpawner = Cast<AFP_ZombieSpawner>(
		UGameplayStatics::GetActorOfClass(GetWorld(), AFP_ZombieSpawner::StaticClass()));
}

void AFP_GameMode::InitGameFlow()
{
	if (!IsValid(FlowControllerClass)) return;
	
	FlowController = NewObject<UFP_FlowController>(this, FlowControllerClass);
	FlowController->Initialize(this);
}

void AFP_GameMode::OnWaveCompleted()
{
	if (!IsValid(FlowController)) return;
	FlowController->StartCountdown();
}

void AFP_GameMode::SubscribeToPlayerFlowControl()
{
	AFP_PlayerCharacter* PlayerCharacter = Cast<AFP_PlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (!IsValid(PlayerCharacter)) return;
	
	PlayerCharacter->OnWantToStart.AddDynamic(this, &AFP_GameMode::BroadcastGameStarted);
}

void AFP_GameMode::BroadcastGameStarted()
{
	OnGameStarted.Broadcast();
}
