// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/Inventory/FP_EquipmentManager.h"

#include "Weapon/WeaponLogic/FP_WeaponBase.h"
#include "Weapon/WeaponVisual/FP_WeaponVisualBase.h"
#include "DataAsset/Weapon/FP_WeaponDataAsset.h"
#include "Character/FP_BaseCharacter.h"
#include "DataAsset/Weapon/FP_MeleeWeaponDataAsset.h"
#include "Engine/Engine.h"
#include "Utils/FP_Sockets.h"
#include "Utils/FP_UtilsFunctionLibrary.h"
#include "Utils/FP_WeaponCurves.h"
#include "Weapon/WeaponLogic/FP_MeleeWeaponBase.h"
#include "Weapon/WeaponVisual/FP_MeleeWeaponVisualBase.h"

UFP_EquipmentManager::UFP_EquipmentManager()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UFP_EquipmentManager::BeginPlay()
{
	Super::BeginPlay();

	GiveStartingWeapons();
}

void UFP_EquipmentManager::GiveStartingWeapons()
{
	AFP_BaseCharacter* OwnerCharacter = Cast<AFP_BaseCharacter>(GetOwner());
	if (!IsValid(OwnerCharacter)) return;

	EquippedWeapons.SetNum(2);

	if (IsValid(StartingPrimaryData))
	{
		EquippedWeapons[0] = CreateLogic(StartingPrimaryData, OwnerCharacter);
	}

	if (IsValid(StartingSecondaryData))
	{
		EquippedWeapons[1] = CreateLogic(StartingSecondaryData, OwnerCharacter);
	}

	if (IsValid(StartingMeleeData))
	{
		CurrentMeleeWeapon = CreateMeleeLogic(StartingMeleeData, OwnerCharacter);
		CurrentMeleeVisual = CreateMeleeVisual(OwnerCharacter);
	}

	if (EquippedWeapons.Num() > 0)
	{
		for (int i = 0; i < EquippedWeapons.Num(); i++)
		{
			if (EquippedWeapons[i] != nullptr)
			{
				EquipByIndex(i);
			}
		}
	}

	BroadcastWeaponInitialized();
}

void UFP_EquipmentManager::EquipNewWeapon(UFP_WeaponDataAsset* NewWeaponData)
{
	if (!IsValid(NewWeaponData)) return;

	AFP_BaseCharacter* OwnerCharacter = Cast<AFP_BaseCharacter>(GetOwner());
	if (!IsValid(OwnerCharacter)) return;

	const int32 SlotIndex = static_cast<int32>(NewWeaponData->WeaponSlot);
	if (!EquippedWeapons.IsValidIndex(SlotIndex)) return;

	EquippedWeapons[SlotIndex] = CreateLogic(NewWeaponData, OwnerCharacter);

	if (SlotIndex == CurrentWeaponIndex)
	{
		CurrentWeapon = nullptr;
		EquipByIndex(SlotIndex);
	}

	OnWeaponAmmoChanged.Broadcast(EquippedWeapons[SlotIndex]);
}

UFP_MeleeWeaponBase* UFP_EquipmentManager::CreateMeleeLogic(UFP_MeleeWeaponDataAsset* MeleeDataAsset,
                                                            AFP_BaseCharacter* OwnerCharacter)
{
	TSubclassOf<UFP_MeleeWeaponBase> LogicClass = MeleeDataAsset->MeleeWeaponLogicClass;
	if (!IsValid(LogicClass))
	{
		LogicClass = UFP_MeleeWeaponBase::StaticClass();
	}

	UFP_MeleeWeaponBase* NewMeleeWeapon = NewObject<UFP_MeleeWeaponBase>(this, LogicClass);

	NewMeleeWeapon->MeleeWeaponData = MeleeDataAsset;
	NewMeleeWeapon->OwningCharacter = OwnerCharacter;
	NewMeleeWeapon->Damage = MeleeDataAsset->Damage;
	NewMeleeWeapon->AttackRange = MeleeDataAsset->AttackRange;
	NewMeleeWeapon->AttackRadius = MeleeDataAsset->AttackRadius;

	return NewMeleeWeapon;
}

AFP_MeleeWeaponVisualBase* UFP_EquipmentManager::CreateMeleeVisual(AFP_BaseCharacter* OwnerCharacter)
{
	if (!IsValid(CurrentMeleeWeapon->MeleeWeaponData->MeleeVisualClass)) return nullptr;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = OwnerCharacter;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AFP_MeleeWeaponVisualBase* NewVisual = GetWorld()->SpawnActor<AFP_MeleeWeaponVisualBase>(
		CurrentMeleeWeapon->MeleeWeaponData->MeleeVisualClass,
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		SpawnParams
	);

	if (!IsValid(NewVisual)) return nullptr;

	NewVisual->Initialize(CurrentMeleeWeapon);

	FName AttachSocket = AttachMeleeTo(CurrentMeleeWeapon);

	FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, true);
	USkeletalMeshComponent* CharMesh = OwnerCharacter->GetMesh();

	if (IsValid(CharMesh) && CharMesh->DoesSocketExist(AttachSocket))
	{
		NewVisual->AttachToComponent(CharMesh, AttachRules, AttachSocket);
	}

	return NewVisual;
}

UFP_WeaponBase* UFP_EquipmentManager::CreateLogic(UFP_WeaponDataAsset* WeaponDataAsset,
                                                  AFP_BaseCharacter* OwnerCharacter)
{
	TSubclassOf<UFP_WeaponBase> LogicClass = WeaponDataAsset->WeaponLogicClass;
	if (!IsValid(LogicClass))
	{
		LogicClass = UFP_WeaponBase::StaticClass();
	}

	UFP_WeaponBase* NewWeapon = NewObject<UFP_WeaponBase>(this, LogicClass);
	NewWeapon->WeaponData = WeaponDataAsset;
	NewWeapon->OwningCharacter = OwnerCharacter;
	
	NewWeapon->CurrentDamage = UFP_UtilsFunctionLibrary::GetStatForLevel(WeaponDataAsset,FP_WeaponCurves::DamageCurve, NewWeapon->CurrentWeaponLevel);
	NewWeapon->CurrentFireRate = UFP_UtilsFunctionLibrary::GetStatForLevel(WeaponDataAsset,FP_WeaponCurves::FireRateCurve, NewWeapon->CurrentWeaponLevel);
	NewWeapon->CurrentReloadTime = UFP_UtilsFunctionLibrary::GetStatForLevel(WeaponDataAsset,FP_WeaponCurves::ReloadTimeCurve, NewWeapon->CurrentWeaponLevel);
	NewWeapon->CurrentWeaponPrice = UFP_UtilsFunctionLibrary::GetStatForLevel(WeaponDataAsset,FP_WeaponCurves::PriceCurve, NewWeapon->CurrentWeaponLevel);
	NewWeapon->CurrentAmmo = UFP_UtilsFunctionLibrary::GetStatForLevel(WeaponDataAsset,FP_WeaponCurves::AmmoCurve, NewWeapon->CurrentWeaponLevel);
	NewWeapon->CurrentReserveAmmo = UFP_UtilsFunctionLibrary::GetStatForLevel(WeaponDataAsset,FP_WeaponCurves::ReserveAmmoCurve, NewWeapon->CurrentWeaponLevel);
	NewWeapon->MaxAmmo = UFP_UtilsFunctionLibrary::GetStatForLevel(WeaponDataAsset,FP_WeaponCurves::AmmoCurve, NewWeapon->CurrentWeaponLevel);
	NewWeapon->MaxReserveAmmo = UFP_UtilsFunctionLibrary::GetStatForLevel(WeaponDataAsset,FP_WeaponCurves::ReserveAmmoCurve, NewWeapon->CurrentWeaponLevel);
	
	NewWeapon->CurrentFireMode = WeaponDataAsset->FireMode;

	return NewWeapon;
}

AFP_WeaponVisualBase* UFP_EquipmentManager::CreateVisual(AFP_BaseCharacter* OwnerCharacter)
{
	if (!IsValid(CurrentWeapon->WeaponData->WeaponVisualClass)) return nullptr;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = OwnerCharacter;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AFP_WeaponVisualBase* NewVisual = GetWorld()->SpawnActor<AFP_WeaponVisualBase>(
		CurrentWeapon->WeaponData->WeaponVisualClass,
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		SpawnParams
	);
	if (!IsValid(NewVisual)) return nullptr;

	NewVisual->Initialize(CurrentWeapon);


	FName AttachSocket = AttachWeaponTo(CurrentWeapon);

	FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, true);
	USkeletalMeshComponent* CharMesh = OwnerCharacter->GetMesh();


	if (IsValid(CharMesh) && CharMesh->DoesSocketExist(AttachSocket))
	{
		NewVisual->AttachToComponent(CharMesh, AttachRules, AttachSocket);
	}


	return NewVisual;
}

void UFP_EquipmentManager::EquipByIndex(int32 Index)
{
	if (!CanSwapWeapon()) return;
	if (!EquippedWeapons.IsValidIndex(Index)) return;
	if (EquippedWeapons[Index] == nullptr) return;
	if (Index == CurrentWeaponIndex && IsValid(CurrentWeapon)) return;

	UnequipCurrent();

	CurrentWeaponIndex = Index;
	CurrentWeapon = EquippedWeapons[Index];

	EquipCurrent();
}

void UFP_EquipmentManager::EquipCurrent()
{
	if (!IsValid(CurrentWeapon) || !IsValid(CurrentWeapon->WeaponData)) return;

	AFP_BaseCharacter* OwnerCharacter = Cast<AFP_BaseCharacter>(GetOwner());
	if (!IsValid(OwnerCharacter)) return;

	CurrentVisual = CreateVisual(OwnerCharacter);

	OnWeaponEquipped.Broadcast(CurrentWeapon);
}

void UFP_EquipmentManager::UnequipCurrent()
{
	if (IsValid(CurrentVisual))
	{
		CurrentVisual->Destroy();
		CurrentVisual = nullptr;
	}

	CurrentWeapon = nullptr;
}

void UFP_EquipmentManager::BroadcastWeaponInitialized()
{
	for (UFP_WeaponBase* Weapon : EquippedWeapons)
	{
		if (IsValid(Weapon) && IsValid(Weapon->WeaponData))
		{
			OnWeaponAmmoChanged.Broadcast(Weapon);
		}
	}
}

void UFP_EquipmentManager::SwitchByDirection(int32 Direction)
{
	if (!CanSwapWeapon()) return;
	if (Direction == 0 || EquippedWeapons.Num() == 0) return;

	int32 NewIndex = (CurrentWeaponIndex + Direction) % EquippedWeapons.Num();
	if (NewIndex < 0) NewIndex += EquippedWeapons.Num();

	EquipByIndex(NewIndex);
}

FName UFP_EquipmentManager::AttachWeaponTo(UFP_WeaponBase* Weapon)
{
	switch (Weapon->WeaponData->WeaponSlot)
	{
	case EWeaponSlot::Primary: return FP_Sockets::PrimaryWeaponSocket;
	case EWeaponSlot::Secondary: return FP_Sockets::SecondaryWeaponSocket;
	default: return NAME_None;
	}
}

FName UFP_EquipmentManager::AttachMeleeTo(UFP_MeleeWeaponBase* MeleeWeapon)
{
	return FP_Sockets::KnifeSocket;
}

void UFP_EquipmentManager::StartFire()
{
	UFP_WeaponBase* Weapon = GetCurrentWeapon();
	if (!IsValid(Weapon)) return;

	Weapon->StartFire();
}

void UFP_EquipmentManager::StopFire()
{
	UFP_WeaponBase* Weapon = GetCurrentWeapon();
	if (!IsValid(Weapon)) return;

	Weapon->StopFire();
}

void UFP_EquipmentManager::Reload()
{
	UFP_WeaponBase* Weapon = GetCurrentWeapon();
	if (!IsValid(Weapon)) return;

	Weapon->Reload();
}

void UFP_EquipmentManager::Stab()
{
	UFP_MeleeWeaponBase* MeleeWeapon = GetCurrentMeleeWeapon();
	if (!IsValid(MeleeWeapon)) return;

	MeleeWeapon->PerformAttack();
}
