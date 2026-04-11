// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FP_StaminaComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStaminaChanged, float, NewStamina);

USTRUCT(BlueprintType)
struct FStaminaData
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stamina|Consumption")
	float StaminaConsumptionCost = 20.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stamina|Consumption")
	float StaminaConsumptionRate = 10.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stamina|Recovery")
	float StaminaRecoveryCost = 20.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stamina|Recovery")
	float StaminaRecoveryRate = 5.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stamina|Recovery")
	float StaminaRecoveryDelay = 2.0f;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FPSGAME_API UFP_StaminaComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFP_StaminaComponent();
	
	void InitializeStamina();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stamina")
	float CurrentStamina = 100.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stamina")
	float MaxStamina = 100.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stamina")
	FStaminaData StaminaData;
	
	UFUNCTION(BlueprintCallable, Category = "Stamina")
	void StartConsumingStamina();
	
	UFUNCTION(BlueprintCallable, Category = "Stamina")
	void ConsumeStamina();
	
	UFUNCTION(BlueprintCallable, Category = "Stamina")
	bool TryConsumeStamina(float Amount);
	
	UFUNCTION(BlueprintCallable, Category = "Stamina")
	void StopConsumeStamina();
	
	UFUNCTION(BlueprintCallable, Category = "Stamina")
	void RegenerateStamina();
	
	/* Getters */
	UFUNCTION(BlueprintCallable, Category = "Stamina")
	FORCEINLINE bool IsEnoughStamina() const { return CurrentStamina > 0.0f; }
	
	UPROPERTY(BlueprintAssignable, Category = "Stamina")
	FOnStaminaChanged OnStaminaChanged;

protected:
	virtual void BeginPlay() override;
	
	FTimerHandle StaminaConsumptionTimer;
	FTimerHandle StaminaRecoveryTimer;
};
