// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSGame/Public/Core/FP_GameMode.h"

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
	
	InitZombieSpawner();
	InitWaveController();
}

void AFP_GameMode::InitWaveController()
{
	WaveController = NewObject<UFP_WaveController>(this, UFP_WaveController::StaticClass());
	WaveController->Initialize(ZombieSpawner);
}

void AFP_GameMode::InitZombieSpawner()
{
	ZombieSpawner = Cast<AFP_ZombieSpawner>(UGameplayStatics::GetActorOfClass(GetWorld(), AFP_ZombieSpawner::StaticClass()));
}
