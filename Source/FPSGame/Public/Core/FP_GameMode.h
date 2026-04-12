// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FP_GameMode.generated.h"

class AFP_ZombieSpawner;
class UFP_WaveController;
/**
 * 
 */
UCLASS()
class FPSGAME_API AFP_GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AFP_GameMode();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
	UFP_WaveController* WaveController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
	AFP_ZombieSpawner* ZombieSpawner;

protected:
	virtual void BeginPlay() override;

	void InitWaveController();
	void InitZombieSpawner();
};
