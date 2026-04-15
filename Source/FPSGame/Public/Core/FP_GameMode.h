// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FP_GameMode.generated.h"

class UFP_FlowController;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameStarted);

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	TSubclassOf<UUserWidget> GameOverWidgetClass;

	UPROPERTY(BlueprintReadOnly, Category = "Spawning")
	UFP_WaveController* WaveController;

	UPROPERTY(BlueprintReadOnly, Category = "Spawning")
	AFP_ZombieSpawner* ZombieSpawner;
	
	UPROPERTY(BlueprintReadOnly, Category = "Spawning")
	UFP_FlowController* FlowController;
	
    UPROPERTY(EditDefaultsOnly, Category = "Flow")
    TSubclassOf<UFP_FlowController> FlowControllerClass;
	
	UFUNCTION(BlueprintCallable, Category = "Spawning")
	void InitWaveController();

	UFUNCTION(BlueprintCallable, Category = "Spawning")
	void InitZombieSpawner();
	
	UFUNCTION(BlueprintCallable, Category = "Spawning")
	void InitGameFlow();

	UFUNCTION(BlueprintCallable, Category = "Spawning")
	FORCEINLINE UFP_WaveController* GetWaveController() const { return WaveController; }

	UFUNCTION(BlueprintCallable, Category = "Spawning")
	FORCEINLINE AFP_ZombieSpawner* GetZombieSpawner() const { return ZombieSpawner; }
	
	UFUNCTION(BlueprintCallable, Category = "Spawning")
	FORCEINLINE UFP_FlowController* GetFlowController() const { return FlowController; }
	
	UPROPERTY(BlueprintAssignable, Category="GameFlow")
	FOnGameStarted OnGameStarted;
	
	UFUNCTION()
	void OnWaveCompleted();
	
protected:
	virtual void BeginPlay() override;

	void SubscribeToPlayerFlowControl();

	UFUNCTION()
	void BroadcastGameStarted();
};
