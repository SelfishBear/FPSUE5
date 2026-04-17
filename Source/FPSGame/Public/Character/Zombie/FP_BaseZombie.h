// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/FP_BaseCharacter.h"
#include "Components/StateTreeComponent.h"
#include "FP_BaseZombie.generated.h"

class UFP_ZombieConfig;

UENUM(BlueprintType)
enum class EZombieType : uint8
{
	Walker UMETA(DisplayName = "Walker"),
	Runner  UMETA(DisplayName = "Runner"),
	Tank    UMETA(DisplayName = "Tank"),
	Crawler  UMETA(DisplayName = "Crawler"),
};

UCLASS()
class FPSGAME_API AFP_BaseZombie : public AFP_BaseCharacter
{
	GENERATED_BODY()

public:
	AFP_BaseZombie(const FObjectInitializer& ObjectInitializer);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config")
	TObjectPtr<UFP_ZombieConfig> ZombieDataAsset;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Config")
	float CurrentAttackTraceDistance;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Config")
	float CurrentAttackRadius;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Config")
	float CurrentAttackDamage;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Config")
	float CurrentMoveSpeed;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Config")
	float CurrentMaxHealth;
	
	UFUNCTION(BlueprintCallable, Category = "Zombie")
	void ReturnToBaseSpeed();
	
	UFUNCTION(BlueprintCallable, Category = "Zombie")
	void SlowDown();
	
	static void ResetMultipliers();
	
	inline static float HealthMultiplier = 1.0f;
	inline static float SpeedMultiplier	= 1.0f;
	inline static float DamageMultiplier = 1.0f;

protected:
	virtual void PostInitializeComponents() override;
	
	virtual void BeginPlay() override;
	
	void InitComponents();
	void InitStats();
};
