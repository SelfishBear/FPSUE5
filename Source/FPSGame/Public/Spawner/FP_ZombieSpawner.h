// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Zombie/FP_BaseZombie.h"
#include "GameFramework/Actor.h"
#include "FP_ZombieSpawner.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnZombiesSpawned, int32, SpawnedZombies);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnZombieDead, int32, LeftZombies);

enum class EZombieType : uint8;

UCLASS()
class FPSGAME_API AFP_ZombieSpawner : public AActor
{
	GENERATED_BODY()

public:
	AFP_ZombieSpawner();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning", meta = (MakeEditWidget = true))
	TArray<FVector> SpawnPoints;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawning")
	TMap<EZombieType, TSubclassOf<AFP_BaseZombie>> ZombieClasses;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	TArray<EZombieType> AllZombieTypes;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	TArray<EZombieType> AvailableZombieTypes;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	int32 CurrentAmountToSpawn = 5;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
	int32 ZombiesLeft = 0;
	
	UFUNCTION(BlueprintCallable, Category = "Spawning")
	void SpawnZombies();
	
	UPROPERTY(BlueprintAssignable, Category = "Spawning")
	FOnZombieDead OnZombieDead;
	
	UPROPERTY(BlueprintAssignable, Category = "Spawning")
	FOnZombiesSpawned OnZombiesSpawned;

	UFUNCTION(BlueprintCallable, Category = "Zombies")
	void UpgradeZombies();

protected:
	UFUNCTION()
	void OnKilledZombie();

	void UpgradeZombiesType();
	void UpgradeZombiesAmount();
	void UpgradeZombiesStats();
};
