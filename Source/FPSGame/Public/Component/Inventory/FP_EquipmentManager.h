// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Weapon/WeaponLogic/FP_WeaponBase.h"
#include "FP_EquipmentManager.generated.h"

class AFP_MeleeWeaponVisualBase;
class UFP_MeleeWeaponBase;
class UFP_MeleeWeaponDataAsset;
class UFP_WeaponBase;
class UFP_WeaponDataAsset;
class AFP_WeaponVisualBase;
class AFP_BaseCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponAmmoChanged, UFP_WeaponBase*, NewWeapon);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponEquiped, UFP_WeaponBase*, NewWeapon);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FPSGAME_API UFP_EquipmentManager final : public UActorComponent
{
	GENERATED_BODY()

public:
	UFP_EquipmentManager();

	void StartFire();
	void StopFire();

	void Reload();

	void Stab();
	
	void EquipByIndex(int32 Index);
	void SwitchByDirection(int32 Direction);

	FName AttachWeaponTo(UFP_WeaponBase* Weapon);
	FName AttachMeleeTo(UFP_MeleeWeaponBase* MeleeWeapon);

	UFUNCTION(BlueprintCallable, Category="Weapon")
	void UpgradeWeapon(int32 Index);
	
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void EquipNewWeapon(UFP_WeaponDataAsset* NewWeaponData);
	
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	bool CanUpgradeWeaponByIndex(int32 Index) const;

	/*Ranged Weapon */
	UFUNCTION(BlueprintPure, Category = "Weapon")
	FORCEINLINE UFP_WeaponBase* GetCurrentWeapon() const { return CurrentWeapon; }

	UFUNCTION(BlueprintPure, Category = "Weapon")
	FORCEINLINE AFP_WeaponVisualBase* GetCurrentWeaponVisual() const { return CurrentVisual; }

	/*Melee Weapon */
	UFUNCTION(BlueprintPure, Category = "Weapon")
	FORCEINLINE UFP_MeleeWeaponBase* GetCurrentMeleeWeapon() const { return CurrentMeleeWeapon; }

	UFUNCTION(BlueprintPure, Category = "Weapon")
	FORCEINLINE AFP_MeleeWeaponVisualBase* GetCurrentMeleeWeaponVisual() const { return CurrentMeleeVisual; }

	UFUNCTION(BlueprintPure, Category = "Inventory")
	FORCEINLINE bool CanSwapWeapon() const { return bCanSwapWeapon; }
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	FORCEINLINE void SetCanSwapWeapon(const bool CanSwap) { bCanSwapWeapon = CanSwap; }

	UFUNCTION(BlueprintPure, Category = "Weapon")
	FORCEINLINE int32 GetCurrentWeaponIndex() const { return CurrentWeaponIndex; }

	UPROPERTY(BlueprintAssignable, Category = "Weapon|Events")
	FOnWeaponAmmoChanged OnWeaponAmmoChanged;

	UPROPERTY(BlueprintAssignable, Category = "Weapon|Events")
	FOnWeaponEquiped OnWeaponEquipped;

	bool bCanSwapWeapon = true;

protected:
	virtual void BeginPlay() override;

	void GiveStartingWeapons();

	void EquipCurrent();
	void UnequipCurrent();

	void BroadcastWeaponInitialized();

	/*Ranged Weapon */
	UFP_WeaponBase* CreateLogic(UFP_WeaponDataAsset* WeaponDataAsset, AFP_BaseCharacter* OwnerCharacter);
	AFP_WeaponVisualBase* CreateVisual(AFP_BaseCharacter* OwnerCharacter);

	/*Melee Weapon */
	UFP_MeleeWeaponBase* CreateMeleeLogic(UFP_MeleeWeaponDataAsset* MeleeDataAsset, AFP_BaseCharacter* OwnerCharacter);
	AFP_MeleeWeaponVisualBase* CreateMeleeVisual(AFP_BaseCharacter* OwnerCharacter);


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Starting")
	TObjectPtr<UFP_WeaponDataAsset> StartingPrimaryData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Starting")
	TObjectPtr<UFP_WeaponDataAsset> StartingSecondaryData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Starting")
	TObjectPtr<UFP_MeleeWeaponDataAsset> StartingMeleeData;

	/* Ranged Weapons */
	UPROPERTY(BlueprintReadOnly, Category = "Weapon")
	TArray<UFP_WeaponBase*> EquippedWeapons;

	UPROPERTY()
	TObjectPtr<AFP_WeaponVisualBase> CurrentVisual;

	UPROPERTY()
	TObjectPtr<UFP_WeaponBase> CurrentWeapon;

	int32 CurrentWeaponIndex = 0;

	/* Melee Weapon */
	UPROPERTY()
	TObjectPtr<AFP_MeleeWeaponVisualBase> CurrentMeleeVisual;

	UPROPERTY()
	TObjectPtr<UFP_MeleeWeaponBase> CurrentMeleeWeapon;
};
