// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/Character/FP_StaminaComponent.h"

UFP_StaminaComponent::UFP_StaminaComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UFP_StaminaComponent::BeginPlay()
{
	Super::BeginPlay();

	InitializeStamina();
}

void UFP_StaminaComponent::InitializeStamina()
{
	CurrentStamina = MaxStamina;
	OnStaminaChanged.Broadcast(CurrentStamina);
}

void UFP_StaminaComponent::StartConsumingStamina()
{
	if (!GetWorld()) return;

	if (GetWorld()->GetTimerManager().IsTimerActive(StaminaRecoveryTimer))
	{
		GetWorld()->GetTimerManager().ClearTimer(StaminaRecoveryTimer);
	}

	GetWorld()->GetTimerManager().SetTimer(StaminaConsumptionTimer, this, &UFP_StaminaComponent::ConsumeStamina,
	                                       StaminaData.StaminaConsumptionRate, true);
}

void UFP_StaminaComponent::ConsumeStamina()
{
	CurrentStamina = FMath::Clamp(CurrentStamina - StaminaData.StaminaConsumptionCost, 0.0f, MaxStamina);
	OnStaminaChanged.Broadcast(CurrentStamina);
}

bool UFP_StaminaComponent::TryConsumeStamina(float Amount)
{
	if (CurrentStamina - Amount < 0.0f) return false;

	CurrentStamina = FMath::Clamp(CurrentStamina - Amount, 0.0f, MaxStamina);
	OnStaminaChanged.Broadcast(CurrentStamina);
	
	StopConsumeStamina();
	return true;
}

void UFP_StaminaComponent::StopConsumeStamina()
{
	if (GetWorld()->GetTimerManager().IsTimerActive(StaminaConsumptionTimer))
	{
		GetWorld()->GetTimerManager().ClearTimer(StaminaConsumptionTimer);
	}

	GetWorld()->GetTimerManager().SetTimer(StaminaRecoveryTimer, this, &UFP_StaminaComponent::RegenerateStamina,
	                                       StaminaData.StaminaRecoveryRate, true, StaminaData.StaminaRecoveryDelay);
}

void UFP_StaminaComponent::RegenerateStamina()
{
	if (CurrentStamina >= MaxStamina)
	{
		OnStaminaChanged.Broadcast(CurrentStamina);
		GetWorld()->GetTimerManager().ClearTimer(StaminaRecoveryTimer);
		return;
	}

	CurrentStamina = FMath::Clamp(CurrentStamina + StaminaData.StaminaRecoveryCost, 0.0f, MaxStamina);
	OnStaminaChanged.Broadcast(CurrentStamina);
}
