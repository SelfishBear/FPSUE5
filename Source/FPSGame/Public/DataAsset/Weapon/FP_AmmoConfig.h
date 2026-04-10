// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FP_AmmoConfig.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EAmmoType : uint8
{
	Primary UMETA(DisplayName = "Primary"),
	Secondary UMETA(DisplayName = "Secondary"),
	Shotgun UMETA(DisplayName = "Shotgun"),
};

UCLASS(BlueprintType)
class FPSGAME_API UFP_AmmoConfig : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo Config")
	FName AmmoName = "DefaultAmmo";
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo Config")
	EAmmoType AmmoType = EAmmoType::Primary;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo Config")
	float Amount = 30;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo Config")
	float Price = 100;
};
