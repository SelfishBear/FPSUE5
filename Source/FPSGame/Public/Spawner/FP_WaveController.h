// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FP_WaveController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnZombiesWaveSpawned, int32, SpawnedZombies);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnZombieWaveDead, int32, LeftZombies);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWaveChanged, int32, CurrentWave);

class AFP_ZombieSpawner;
/**
 * 
 */
UCLASS()
class FPSGAME_API UFP_WaveController : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	AFP_ZombieSpawner* ZombieSpawner;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave")
	int32 CurrentWave = 0;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave")
	int32 CurrentSpawnedZombies = 0;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave")
	int32 CurrentZombiesLeft = 0;
	
	UFUNCTION(BlueprintCallable, Category = "Wave")
	void StartNextWave();

	void Initialize(AFP_ZombieSpawner* Spawner);
	
	UPROPERTY(BlueprintAssignable, Category = "Wave")
	FOnZombieWaveDead OnZombieDeadDelegate;
	
	UPROPERTY(BlueprintAssignable, Category = "Wave")
	FOnZombiesWaveSpawned OnZombiesSpawnedDelegate;
	
	UPROPERTY(BlueprintAssignable, Category = "Wave")
	FOnWaveChanged OnWaveChangedDelegate;
	
	UFUNCTION(BlueprintCallable, Category = "Zombies")
	void RewardPlayer();

protected:
	UFUNCTION()
	void OnZombiesSpawned(int32 SpawnedZombies);

	UFUNCTION()
	void OnZombieDead(int32 LeftZombies);
	
	void CheckWaveCompletion();
	
};
