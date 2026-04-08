// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/Character/FP_HealthComponent.h"


UFP_HealthComponent::UFP_HealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UFP_HealthComponent::BeginPlay()
{
	Super::BeginPlay();

	InitializeHealth();
}

void UFP_HealthComponent::InitializeHealth()
{
	CurrentHealth = MaxHealth;
	OnHealthChanged.Broadcast(CurrentHealth);
}

void UFP_HealthComponent::TakeDamage(float DamageAmount)
{
	if (!GetWorld()) return;	
	
	ActivateAutoHeal();
	
	float NewHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.0f, MaxHealth);
	if (NewHealth == CurrentHealth) return;

	CurrentHealth = NewHealth;
	OnHealthChanged.Broadcast(CurrentHealth);
	
	Die();
}

void UFP_HealthComponent::HandleHealTimer()
{
	Regenerate();
}

void UFP_HealthComponent::ActivateAutoHeal()
{
	if(AutoHealSettings.bAutoHealEnabled)
	{
		if (GetWorld()->GetTimerManager().IsTimerActive(AutoHealthTimer))
		{
			GetWorld()->GetTimerManager().ClearTimer(AutoHealthTimer);
		}
		
		GetWorld()->GetTimerManager().SetTimer(AutoHealthTimer, this, &UFP_HealthComponent::HandleHealTimer, AutoHealSettings.HealInterval, true, AutoHealSettings.HealDelay);
	}
}

void UFP_HealthComponent::Die()
{
	if (CurrentHealth <= 0.0f && !bIsDead)
	{
		bIsDead = true;
		OnDeath.Broadcast(bIsDead);
	}
}

void UFP_HealthComponent::Regenerate()
{
	if (CurrentHealth >= MaxHealth)
	{
		GetWorld()->GetTimerManager().ClearTimer(AutoHealthTimer);
		return;
	}
	
	float NewHealth = FMath::Clamp(CurrentHealth + AutoHealSettings.HealAmount, 0.0f, MaxHealth);
	if (NewHealth == CurrentHealth) return;

	CurrentHealth = NewHealth;
	OnHealthChanged.Broadcast(CurrentHealth);
}
