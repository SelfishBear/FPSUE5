// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataAsset/Weapon/FP_WeaponDataAsset.h"
#include "UObject/Object.h"
#include "Weapon/WeaponTypes/FP_WeaponTypes.h"
#include "FP_WeaponBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFire);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReload);

class UFP_WeaponDataAsset;
class AFP_BaseCharacter;
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class FPSGAME_API UFP_WeaponBase : public UObject
{
	GENERATED_BODY()

public:
	TWeakObjectPtr<AFP_BaseCharacter> OwningCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<UFP_WeaponDataAsset> WeaponData;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	EWeaponState CurrentState = EWeaponState::Idle;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	EFireMode CurrentFireMode = EFireMode::FullAuto;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	bool bCanFire = true;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	bool bCanReload = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float TraceDistance = 10000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (ClampMin = "1", ClampMax = "5"))
	int32 CurrentWeaponLevel = 1;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="WeaponStats")
	float CurrentDamage = 20.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="WeaponStats")
	float CurrentFireRate = 600.0f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="WeaponStats")
	float CurrentReloadTime = 1.0f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="WeaponStats")
	float CurrentWeaponPrice = 200.0f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="WeaponStats")
	float CurrentWeaponUpgradePrice = 30.0f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WeaponStats")
	float CurrentAmmo = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WeaponStats")
	float CurrentReserveAmmo = 0.0f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WeaponStats")
	float MaxAmmo = 30.0f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WeaponStats")
	float MaxReserveAmmo = 90.0f;
	
	void StartFire();
	void StopFire();

	void Reload();
	void FinishReload();

	void PerformFire();

	void PrintDebugMessage(FHitResult HitResult, bool bHit);

	void TryApplyDamage(const FHitResult& HitResult, bool bHit);
	
	/* Virtual Methods */
	virtual void PerformFireLogic();

	virtual void ConsumeAmmo();
	
	/* Getters */
	UFUNCTION(BlueprintPure, Category = "Weapon")
	FORCEINLINE AFP_BaseCharacter* GetOwningCharacter() const { return OwningCharacter.Get(); }

	UFUNCTION(BlueprintPure, Category = "Weapon")
	FORCEINLINE UFP_WeaponDataAsset* GetWeaponData() const { return WeaponData; }

	UFUNCTION(BlueprintPure, Category = "Weapon")
	FORCEINLINE float GetCurrentAmmo() const { return CurrentAmmo; }

	UFUNCTION(BlueprintPure, Category = "Weapon")
	FORCEINLINE float GetCurrentReserveAmmo() const { return CurrentReserveAmmo; }

	UFUNCTION(BlueprintPure, Category = "Weapon")
	FORCEINLINE bool CanFire() const { return bCanFire && CurrentAmmo > 0 && CurrentState != EWeaponState::Reloading; }
	
	UFUNCTION(BlueprintPure, Category = "Weapon")
	FORCEINLINE bool CanReload() const { return bCanReload && CurrentAmmo < MaxAmmo && CurrentReserveAmmo > 0 && CurrentState != EWeaponState::Reloading; }

	UFUNCTION(BlueprintPure, Category = "Weapon")
	FORCEINLINE EWeaponState GetCurrentState() const { return CurrentState; }

	UFUNCTION(BlueprintPure, Category = "Weapon")
	FORCEINLINE EFireMode GetCurrentFireMode() const { return CurrentFireMode; }

	UFUNCTION(BlueprintPure, Category = "Weapon")
	FORCEINLINE EWeaponSlot GetWeaponSlot() const { return WeaponData->WeaponSlot; }

	virtual UWorld* GetWorld() const override;

	UPROPERTY(BlueprintAssignable, Category = "Weapon|Events")
	FOnFire OnFire;

	UPROPERTY(BlueprintAssignable, Category = "Weapon|Events")
	FOnReload OnReload;

protected:
	virtual bool MakeTrace(FHitResult& OutHitResult) const;

	void BroadcastAmmoChanged();

	FTimerHandle FireTimerHandle;

	int32 BurstShotsRemaining = 0;

	bool bIsFiring = false;

private:
	void HandleFireTimer();
};
