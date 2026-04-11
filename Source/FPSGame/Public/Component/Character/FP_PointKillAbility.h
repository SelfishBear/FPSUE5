// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NiagaraComponent.h"
#include "FP_PointKillAbility.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FPSGAME_API UFP_PointKillAbility : public UActorComponent
{
	GENERATED_BODY()

public:
	UFP_PointKillAbility();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PointKill")
	float PointKillDamage = 150.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PointKill")
	float PointKillCooldown = 15.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PointKill")
	TObjectPtr<UAnimMontage> PointKillMontage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PointKill")
	TObjectPtr<UNiagaraSystem> PointKillFX;

	UFUNCTION(BlueprintCallable, Category = "PointKill")
	void StartPointKill();

	UFUNCTION(BlueprintCallable, Category = "PointKill")
	void PerformPointKill();

	bool bCanPerformPointKill = true;

private:
	FTimerHandle FP_PointKillTimerHandle;
};
