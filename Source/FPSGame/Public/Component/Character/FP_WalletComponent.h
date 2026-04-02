// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FP_WalletComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMoneyChanged, float, NewAmount);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FPSGAME_API UFP_WalletComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFP_WalletComponent();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float CurrentMoney = 0.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float StartingMoney = 1000.0f;
	
	UFUNCTION(BlueprintCallable)
	void AddMoney(float Amount);
	
	UFUNCTION(BlueprintCallable)
	bool TrySpendMoney(float Amount);

	UPROPERTY(BlueprintAssignable)
	FOnMoneyChanged OnMoneyChanged;
protected:
	virtual void BeginPlay() override;

	void InitializeMoney();
};
