// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/Character/FP_WalletComponent.h"

UFP_WalletComponent::UFP_WalletComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UFP_WalletComponent::BeginPlay()
{
	Super::BeginPlay();

	InitializeMoney();
}

void UFP_WalletComponent::InitializeMoney()
{
	CurrentMoney = StartingMoney;
	OnMoneyChanged.Broadcast(CurrentMoney);
}

void UFP_WalletComponent::AddMoney(float Amount)
{
	if (Amount <= 0.0f) return;

	CurrentMoney += Amount;
	OnMoneyChanged.Broadcast(CurrentMoney);
}

bool UFP_WalletComponent::TrySpendMoney(float Amount)
{
	if (CurrentMoney - Amount < 0.0f) return false;

	CurrentMoney -= Amount;
	OnMoneyChanged.Broadcast(CurrentMoney);
	return true;
}
