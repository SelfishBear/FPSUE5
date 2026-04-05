// Fill out your copyright notice in the Description page of Project Settings.


#include "Utils/FP_UtilsFunctionLibrary.h"

#include "DataAsset/Weapon/FP_WeaponDataAsset.h"

float UFP_UtilsFunctionLibrary::GetStatForLevel(UFP_WeaponDataAsset* WeaponData, FName StatName, int32 Level)
{
	if (!IsValid(WeaponData)) return 0.0f;
	
	static const FString ContextString = TEXT("WeaponLevelingContext");
	FRealCurve* Curve = WeaponData->WeaponLevelingTable->FindCurve(StatName, ContextString);
	if (!Curve) return 0.0f;	
	return Curve->Eval(static_cast<float>(Level));
}
