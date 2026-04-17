// Fill out your copyright notice in the Description page of Project Settings.


#include "Spawner/FP_WaveController.h"

#include "MovieSceneSequencePlaybackSettings.h"
#include "Character/FP_PlayerCharacter.h"
#include "Component/Character/FP_WalletComponent.h"
#include "Core/FP_GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Spawner/FP_ZombieSpawner.h"
#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "Component/Character/FP_AudioComponent.h"
#include "Core/FP_GameMode.h"
#include "Subsystem/FP_MusicSubsystem.h"


class UFP_MusicSubsystem;
class UFP_GameInstance;
class AFP_PlayerCharacter;

void UFP_WaveController::Initialize(AFP_ZombieSpawner* Spawner)
{
	ZombieSpawner = Spawner;
	ZombieSpawner->OnZombiesSpawned.AddDynamic(this, &UFP_WaveController::OnZombiesSpawned);
	ZombieSpawner->OnZombieDead.AddDynamic(this, &UFP_WaveController::OnZombieDead);

	PlayChillMusic();
}

void UFP_WaveController::StartNextWave()
{
	CurrentWave++;
	ZombieSpawner->SpawnZombies();
	OnWaveChangedDelegate.Broadcast(CurrentWave);

	PlayFightMusic();
}

void UFP_WaveController::RewardPlayer()
{
	if (!GetWorld()) return;

	UFP_GameInstance* FP_GameInstance = Cast<UFP_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (!FP_GameInstance) return;

	AFP_PlayerCharacter* PlayerCharacter = Cast<AFP_PlayerCharacter>(
		UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (!IsValid(PlayerCharacter)) return;

	PlayerCharacter->GetWalletComponent()->AddMoney(CurrentWave + (10 * FP_GameInstance->DifficultyMultiplier));
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
	
	
	if (!GetWorld()) return;

	UFP_GameInstance* FP_GameInstance = Cast<UFP_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (!FP_GameInstance) return;

	AFP_PlayerCharacter* PlayerCharacter = Cast<AFP_PlayerCharacter>(
		UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (!IsValid(PlayerCharacter)) return;
	
	PlayerCharacter->GetFP_AudioComponent()->PlaySoundWithChance(PlayerCharacter->GetFP_AudioComponent()->TauntPhrases, 25.0f);
}

void UFP_WaveController::CheckWaveCompletion()
{
	UE_LOG(LogTemp, Warning, TEXT("[WaveController] CheckWaveCompletion called. CurrentZombiesLeft = %d"),
	       CurrentZombiesLeft);
	if (CurrentZombiesLeft > 0) return;

	AFP_GameMode* GameMode = Cast<AFP_GameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (!IsValid(GameMode)) return;
	
	UE_LOG(LogTemp, Warning, TEXT("[WaveController] Wave completed! Calling UpgradeZombies."));
	if (CurrentWave % 3 == 0)
	{
		ZombieSpawner->UpgradeZombies();
	}
	
	if (CurrentWave == GameMode->WaveEndEdge)
	{
		AActor* Merchant = UGameplayStatics::GetActorOfClass(GetWorld(), GameMode->MerchantClass);
		Merchant->Destroy();
		
		ALevelSequenceActor* OutActor;

		ULevelSequencePlayer* SequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(
			GetWorld(),
			GameMode->WaveEndSequence,
			FMovieSceneSequencePlaybackSettings(),
			OutActor
		);
		
		SequencePlayer->Play();
	}
	
	OnWaveCompleted.Broadcast();
	PlayChillMusic();
}

void UFP_WaveController::PlayFightMusic()
{
	if (!GetWorld()) return;

	UFP_MusicSubsystem* MusicSubSystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<
		UFP_MusicSubsystem>();

	if (!IsValid(MusicSubSystem)) return;

	MusicSubSystem->PlayFightMusic();
}

void UFP_WaveController::PlayChillMusic()
{
	if (!GetWorld()) return;

	UFP_MusicSubsystem* MusicSubSystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<
		UFP_MusicSubsystem>();

	if (!IsValid(MusicSubSystem)) return;

	MusicSubSystem->PlayChillMusic();
}
