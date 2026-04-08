// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "FP_Damageable.generated.h"

UINTERFACE(Blueprintable, BlueprintType)
class UFP_Damageable : public UInterface
{
	GENERATED_BODY()
};

class FPSGAME_API IFP_Damageable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damage")
	void ReceiveDamage(float DamageAmount);
};
