// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Weapon/WeaponTypes/FP_WeaponTypes.h"
#include "FP_WeaponDataAsset.generated.h"

class AFP_WeaponVisualBase;
class UFP_WeaponBase;
/**
 * 
 */
UCLASS()
class FPSGAME_API UFP_WeaponDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="WeaponSettings")
	FName WeaponName = "DefaultWeapon";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="WeaponSettings")
	EWeaponSlot WeaponSlot = EWeaponSlot::Primary;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="WeaponSettings")
	float Damage = 20.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="WeaponSettings")
	float FireRate = 600.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="WeaponSettings")
	float MaxAmmo = 30.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="WeaponSettings")
	float MaxReserveAmmo = 90.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="WeaponSettings")
	float SpreadAngle = 5.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="WeaponSettings")
	EFireMode FireMode = EFireMode::FullAuto;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="WeaponSettings")
	TSubclassOf<AFP_WeaponVisualBase> WeaponVisualClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="WeaponSettings")
	TSubclassOf<UFP_WeaponBase> WeaponLogicClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animations")
	TObjectPtr<UAnimMontage> FireMontage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animations")
	TObjectPtr<UAnimMontage> ReloadMontage;
};
