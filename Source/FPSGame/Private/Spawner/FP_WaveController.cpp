// Fill out your copyright notice in the Description page of Project Settings.


#include "Spawner/FP_WaveController.h"

#include "Character/FP_PlayerCharacter.h"
#include "Component/Character/FP_WalletComponent.h"
#include "Core/FP_GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Spawner/FP_ZombieSpawner.h"


class UFP_GameInstance;
class AFP_PlayerCharacter;

void UFP_WaveController::Initialize(AFP_ZombieSpawner* Spawner)
{
	ZombieSpawner = Spawner;
	ZombieSpawner->OnZombiesSpawned.AddDynamic(this, &UFP_WaveController::OnZombiesSpawned);
	ZombieSpawner->OnZombieDead.AddDynamic(this, &UFP_WaveController::OnZombieDead);
}

void UFP_WaveController::StartNextWave()
{
	CurrentWave++;
	ZombieSpawner->SpawnZombies();
	OnWaveChangedDelegate.Broadcast(CurrentWave);
}

void UFP_WaveController::RewardPlayer()
{
	if (!GetWorld()) return;

	UFP_GameInstance* FP_GameInstance = Cast<UFP_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (!FP_GameInstance) return;

	AFP_PlayerCharacter* PlayerCharacter = Cast<AFP_PlayerCharacter>(
		UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (!IsValid(PlayerCharacter)) return;

	PlayerCharacter->GetWalletComponent()->AddMoney(CurrentWave * FP_GameInstance->DifficultyMultiplier);
}

void UFP_WaveController::OnZombiesSpawned(int32 SpawnedZombies)
{
	CurrentSpawnedZombies = SpawnedZombies;
	OnZombiesSpawnedDelegate.Broadcast(CurrentSpawnedZombies);
}

void UFP_WaveController::OnZombieDead(int32 LeftZombies)
{
	CurrentZombiesLeft = LeftZombies;
	OnZombieDeadDelegate.Broadcast(CurrentZombiesLeft);
	RewardPlayer();
	CheckWaveCompletion();
}

void UFP_WaveController::CheckWaveCompletion()
{
	if (CurrentZombiesLeft > 0) return;

	ZombieSpawner->UpgradeZombies();
	OnWaveCompleted.Broadcast();
}
