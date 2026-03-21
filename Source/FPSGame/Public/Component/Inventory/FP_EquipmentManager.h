// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Weapon/WeaponTypes/FP_WeaponTypes.h"
#include "FP_EquipmentManager.generated.h"

class UFP_WeaponBase;
class UFP_WeaponDataAsset;
class AFP_WeaponVisualBase;
class AFP_BaseCharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FPSGAME_API UFP_EquipmentManager : public UActorComponent
{
	GENERATED_BODY()

public:
	UFP_EquipmentManager();

	void AddWeapon(UFP_WeaponDataAsset* WeaponDataAsset);

	void SwitchWeapon(EWeaponSlot Slot);

	void StartFire();
	void StopFire();
	void Reload();

	UFUNCTION(BlueprintPure, Category = "Weapon")
	UFP_WeaponBase* GetCurrentWeapon() const;

	UFUNCTION(BlueprintPure, Category = "Weapon")
	AFP_WeaponVisualBase* GetCurrentVisual() const { return ActiveVisual; }

	UFUNCTION(BlueprintPure, Category = "Weapon")
	EWeaponSlot GetActiveSlot() const { return ActiveSlot; }

protected:
	virtual void BeginPlay() override;

	void GiveStartingWeapons();

	void EquipFromSlot(EWeaponSlot Slot);

	void UnequipCurrent();

	UFP_WeaponBase* CreateLogic(UFP_WeaponDataAsset* WeaponDataAsset, AFP_BaseCharacter* OwnerCharacter);
	AFP_WeaponVisualBase* CreateVisual(UFP_WeaponDataAsset* WeaponDataAsset, AFP_BaseCharacter* OwnerCharacter);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Starting")
	TObjectPtr<UFP_WeaponDataAsset> StartingPrimaryData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Starting")
	TObjectPtr<UFP_WeaponDataAsset> StartingSecondaryData;

	UPROPERTY()
	TMap<EWeaponSlot, TObjectPtr<UFP_WeaponBase>> WeaponSlots;

	EWeaponSlot ActiveSlot = EWeaponSlot::Primary;

	UPROPERTY()
	TObjectPtr<AFP_WeaponVisualBase> ActiveVisual;
};
