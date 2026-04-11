// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FP_HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChanged, float, NewHealth);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeath, bool, bIsDead);

USTRUCT(Blueprintable)
struct FAutoHealSettings
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AutoHeal")
	bool bAutoHealEnabled = true;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AutoHeal", meta = (EditCondition = "bAutoHealEnabled"))
	float HealAmount = 5.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AutoHeal", meta = (EditCondition = "bAutoHealEnabled"))
	float HealInterval = 1.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AutoHeal", meta = (EditCondition = "bAutoHealEnabled"))
	float HealDelay = 3.0f;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FPSGAME_API UFP_HealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFP_HealthComponent();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Health")
	float CurrentHealth = 100.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Health")
	float MaxHealth = 100.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
	bool bIsDead = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
	FAutoHealSettings AutoHealSettings;

	UFUNCTION(BlueprintCallable, Category = "Health")
	void TakeDamage(float DamageAmount);

	UFUNCTION(BlueprintCallable, Category = "Health")
	void Regenerate();

	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnHealthChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnDeath OnDeath;

protected:
	virtual void BeginPlay() override;

	void InitializeHealth();

	void Die();

	void ActivateAutoHeal();

	void HandleHealTimer();

	void CheckDeath();
	
	FTimerHandle AutoHealthTimer;
};
