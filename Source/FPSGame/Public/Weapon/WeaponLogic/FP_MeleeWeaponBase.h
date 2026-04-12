// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FP_MeleeWeaponBase.generated.h"

class AFP_BaseCharacter;
class UFP_MeleeWeaponDataAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttack);

UCLASS(Blueprintable, BlueprintType)
class FPSGAME_API UFP_MeleeWeaponBase : public UObject
{
	GENERATED_BODY()

public:
	void PerformAttack();
	
	void MakeAttack();
	
	void TryApplyDamage(const FHitResult& HitResult, bool bHit);
	
	UFUNCTION(BlueprintPure, Category = "Melee")
	FORCEINLINE bool CanPerformAttack() const { return bCanAttack; }
	
	UFUNCTION(BlueprintPure, Category = "Weapon")
	FORCEINLINE AFP_BaseCharacter* GetOwningCharacter() const { return OwningCharacter.Get(); }

	UFUNCTION(BlueprintPure, Category = "Weapon")
	FORCEINLINE UFP_MeleeWeaponDataAsset* GetMeleeWeaponData() const { return MeleeWeaponData; }
	
	TWeakObjectPtr<AFP_BaseCharacter> OwningCharacter;

	UPROPERTY(EditDefaultsOnly, Category = "Melee")
	TObjectPtr<UFP_MeleeWeaponDataAsset> MeleeWeaponData;
	
	UPROPERTY(VisibleAnywhere, Category = "Melee")
	float Damage = 50.0f;
	
	UPROPERTY(VisibleAnywhere, Category = "Melee")
	float AttackRange = 200.0f;
	
	UPROPERTY(VisibleAnywhere, Category = "Melee")
	float AttackRadius = 50.0f;
	
	UPROPERTY(VisibleAnywhere, Category = "Melee")
	bool bCanAttack = true;
	
	UPROPERTY(BlueprintAssignable, Category = "Weapon|Events")
	FOnAttack OnAttack;
	
protected:
	virtual bool MakeTrace(FHitResult& OutHitResult) const;
	
	FTimerHandle AttackCooldownTimerHandle;
};
