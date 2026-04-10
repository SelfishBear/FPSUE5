// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Zombie/FP_BaseZombie.h"
#include "Engine/DataAsset.h"
#include "FP_ZombieConfig.generated.h"

/**
 * 
 */
UCLASS()
class FPSGAME_API UFP_ZombieConfig : public UDataAsset
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zombie Config")
	EZombieType ZombieType = EZombieType::Walker;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zombie Config")
	float AttackTraceDistance = 150.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zombie Config")
	float AttackRadius = 15.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zombie Config")
	float AttackDamage = 20.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zombie Config")
	float MoveSpeed = 150.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zombie Config")
	float MaxHealth = 100.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zombie Config")
	TArray<TObjectPtr<UAnimMontage>> DeathMontages;
};
