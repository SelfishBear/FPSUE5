// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FP_UtilsFunctionLibrary.generated.h"

class UFP_WeaponDataAsset;
/**
 * 
 */
UCLASS()
class FPSGAME_API UFP_UtilsFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category = "Weapon Utils")
	static float GetStatForLevel(UFP_WeaponDataAsset* WeaponData, FName StatName, int32 Level);
};
