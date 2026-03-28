// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FP_MeleeWeaponDataAsset.generated.h"

class UFP_MeleeWeaponBase;
/**
 * 
 */
UCLASS()
class FPSGAME_API UFP_MeleeWeaponDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category="Melee")
	TSubclassOf<AActor> MeleeVisualClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Melee")
	TSubclassOf<UFP_MeleeWeaponBase> MeleeWeaponLogicClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Melee")
	TObjectPtr<UAnimMontage> AttackMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Melee")
	float Damage = 50.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Melee")
	float AttackRange = 200.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Melee")
	float AttackRadius = 50.0f;
};
