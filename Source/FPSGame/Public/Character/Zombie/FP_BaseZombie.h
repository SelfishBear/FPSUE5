// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/FP_BaseCharacter.h"
#include "Components/StateTreeComponent.h"
#include "FP_BaseZombie.generated.h"

class UFP_ZombieConfig;
// class UFP_AbilityComponent;

UCLASS()
class FPSGAME_API AFP_BaseZombie : public AFP_BaseCharacter
{
	GENERATED_BODY()

public:
	AFP_BaseZombie(const FObjectInitializer& ObjectInitializer);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStateTreeComponent> StateTreeComponent;

	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	// TObjectPtr<UFP_AbilityComponent> AbilityComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config")
	TObjectPtr<UFP_ZombieConfig> ZombieDataAsset;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config")
	float CurrentAttackTraceDistance;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config")
	float CurrentAttackRadius;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config")
	float CurrentAttackDamage;

protected:
	virtual void BeginPlay() override;
	
	void InitStats();
};
