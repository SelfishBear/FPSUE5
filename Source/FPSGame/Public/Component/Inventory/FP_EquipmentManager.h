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
	
	void StartFire();
	void StopFire();
	void Reload();

	void EquipByIndex(int32 Index);
	void SwitchByDirection(int32 Direction);
	
	FName AttachWeaponTo(UFP_WeaponBase* Weapon);

	UFUNCTION(BlueprintPure, Category = "Weapon")
	FORCEINLINE UFP_WeaponBase* GetCurrentWeapon() const { return CurrentWeapon; }; 

	UFUNCTION(BlueprintPure, Category = "Weapon")
	FORCEINLINE AFP_WeaponVisualBase* GetCurrentVisual() const { return CurrentVisual; }
	
	UFUNCTION(BlueprintPure, Category = "Weapon")
	FORCEINLINE int32 GetCurrentWeaponIndex() const { return CurrentWeaponIndex; }

protected:
	virtual void BeginPlay() override;

	void GiveStartingWeapons();

	void EquipCurrent();
	void UnequipCurrent();

	UFP_WeaponBase* CreateLogic(UFP_WeaponDataAsset* WeaponDataAsset, AFP_BaseCharacter* OwnerCharacter);
	AFP_WeaponVisualBase* CreateVisual(UFP_WeaponDataAsset* WeaponDataAsset, AFP_BaseCharacter* OwnerCharacter);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Starting")
	TObjectPtr<UFP_WeaponDataAsset> StartingPrimaryData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Starting")
	TObjectPtr<UFP_WeaponDataAsset> StartingSecondaryData;

	UPROPERTY()
	TArray<UFP_WeaponBase*> EquippedWeapons;
	
	UPROPERTY()
	TObjectPtr<AFP_WeaponVisualBase> CurrentVisual;
	
	UPROPERTY()
	TObjectPtr<UFP_WeaponBase> CurrentWeapon;
	
	int32 CurrentWeaponIndex = 0;
};
