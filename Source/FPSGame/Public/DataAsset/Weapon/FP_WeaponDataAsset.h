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
UCLASS(BlueprintType)
class FPSGAME_API UFP_WeaponDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	/*Weapon Stats*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="WeaponSettings")
	FName WeaponName = "DefaultWeapon";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="WeaponSettings")
	EWeaponSlot WeaponSlot = EWeaponSlot::Primary;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="WeaponSettings")
	float SpreadAngle = 5.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="WeaponSettings")
	EFireMode FireMode = EFireMode::FullAuto;

	/*References to Visual and Logic Classes*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="WeaponSettings")
	TSubclassOf<AFP_WeaponVisualBase> WeaponVisualClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="WeaponSettings")
	TSubclassOf<UFP_WeaponBase> WeaponLogicClass;

	/*Animations*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animations")
	TObjectPtr<UAnimMontage> FireMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animations")
	TObjectPtr<UAnimMontage> ReloadMontage;

	/*FX*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="FX")
	TObjectPtr<UParticleSystem> MuzzleFlashFX;

	/*Sounds*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sounds")
	TObjectPtr<USoundBase> FireSound;

	/*Curve Table*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Curves")
	TObjectPtr<UCurveTable> WeaponLevelingTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Curves")
	int32 MaxLevel = 5;

	/*UI*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponSettings")
	TObjectPtr<UTexture2D> WeaponIcon;
};
