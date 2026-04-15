// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FP_FlowController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCountdownTick, float, RemainingTime);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCountdownStarted);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCountdownFinished);

USTRUCT()
struct FCountdownData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = "Countdown")
	int32 CountdownTime = 60;
};

class AFP_GameMode;
/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class FPSGAME_API UFP_FlowController : public UObject
{
	GENERATED_BODY()
	
public:
	void Initialize(AFP_GameMode* InGameMode);
	
	UPROPERTY(EditAnywhere, Category = "Countdown")
	FCountdownData CountdownData;
	
	UFUNCTION()
	void StartGame();

	UFUNCTION(BlueprintCallable, Category = "Spawning")
	void StartCountdown();

	UPROPERTY(BlueprintAssignable, Category="GameFlow")
	FOnCountdownTick OnCountdownTick;
	
	UPROPERTY(BlueprintAssignable, Category="GameFlow")
	FOnCountdownFinished OnCountdownFinished;
	
	UPROPERTY(BlueprintAssignable, Category="GameFlow")
	FOnCountdownStarted OnCountdownStarted;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="GameFlow")
	bool bCanSkipCountdown = true;

protected:
	UPROPERTY()
	AFP_GameMode* GameMode;

	void UpdateCountdownWidget();

private:
	FTimerHandle CountdownTimerHandle;
	
	FTimerHandle CountdownTickTimerHandle;
};
