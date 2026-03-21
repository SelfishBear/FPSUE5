// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/Inventory/FP_EquipmentManager.h"

#include "Weapon/WeaponLogic/FP_WeaponBase.h"
#include "Weapon/WeaponVisual/FP_WeaponVisualBase.h"
#include "DataAsset/Weapon/FP_WeaponDataAsset.h"
#include "Character/FP_BaseCharacter.h"
#include "Engine/Engine.h"

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
	if (StartingPrimaryData)
	{
		AddWeapon(StartingPrimaryData);
	}

	if (StartingSecondaryData)
	{
		AddWeapon(StartingSecondaryData);
	}

	if (WeaponSlots.Contains(EWeaponSlot::Primary))
	{
		EquipFromSlot(EWeaponSlot::Primary);
	}
	else if (WeaponSlots.Contains(EWeaponSlot::Secondary))
	{
		EquipFromSlot(EWeaponSlot::Secondary);
	}
}

void UFP_EquipmentManager::AddWeapon(UFP_WeaponDataAsset* WeaponDataAsset)
{
	if (!IsValid(WeaponDataAsset)) return;

	AFP_BaseCharacter* OwnerCharacter = Cast<AFP_BaseCharacter>(GetOwner());
	if (!IsValid(OwnerCharacter)) return;

	const EWeaponSlot Slot = WeaponDataAsset->WeaponSlot;

	if (WeaponSlots.Contains(Slot))
	{
		UFP_WeaponBase* OldWeapon = WeaponSlots[Slot];
		if (OldWeapon)
		{
			OldWeapon->StopFire();
		}

		if (ActiveSlot == Slot)
		{
			UnequipCurrent();
		}
	}

	UFP_WeaponBase* NewWeapon = CreateLogic(WeaponDataAsset, OwnerCharacter);
	WeaponSlots.Add(Slot, NewWeapon);
}

void UFP_EquipmentManager::SwitchWeapon(EWeaponSlot Slot)
{
	if (Slot == ActiveSlot) return;
	if (!WeaponSlots.Contains(Slot)) return;

	UnequipCurrent();
	EquipFromSlot(Slot);
}

void UFP_EquipmentManager::EquipFromSlot(EWeaponSlot Slot)
{
	TObjectPtr<UFP_WeaponBase>* FoundWeapon = WeaponSlots.Find(Slot);
	if (!FoundWeapon || !IsValid(*FoundWeapon)) return;

	AFP_BaseCharacter* OwnerCharacter = Cast<AFP_BaseCharacter>(GetOwner());
	if (!IsValid(OwnerCharacter)) return;

	ActiveSlot = Slot;

	UFP_WeaponBase* Weapon = *FoundWeapon;

	if (Weapon->WeaponData)
	{
		ActiveVisual = CreateVisual(Weapon->WeaponData, OwnerCharacter);

		if (ActiveVisual)
		{
			ActiveVisual->Initialize(Weapon);
		}
	}

	if (GEngine)
	{
		const FString SlotName = (Slot == EWeaponSlot::Primary) ? TEXT("Primary") : TEXT("Secondary");
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green,
			FString::Printf(TEXT("Equipped [%s]: %s"), *SlotName, *Weapon->WeaponData->WeaponName.ToString()));
	}
}

void UFP_EquipmentManager::UnequipCurrent()
{
	UFP_WeaponBase* CurrentWeapon = GetCurrentWeapon();
	if (CurrentWeapon)
	{
		CurrentWeapon->StopFire();
	}

	if (ActiveVisual)
	{
		ActiveVisual->Destroy();
		ActiveVisual = nullptr;
	}
}

UFP_WeaponBase* UFP_EquipmentManager::GetCurrentWeapon() const
{
	const TObjectPtr<UFP_WeaponBase>* Found = WeaponSlots.Find(ActiveSlot);
	return Found ? Found->Get() : nullptr;
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
	NewWeapon->CurrentAmmo = WeaponDataAsset->MaxAmmo;
	NewWeapon->CurrentReserveAmmo = WeaponDataAsset->MaxReserveAmmo;
	NewWeapon->CurrentFireMode = WeaponDataAsset->FireMode;

	return NewWeapon;
}

AFP_WeaponVisualBase* UFP_EquipmentManager::CreateVisual(UFP_WeaponDataAsset* WeaponDataAsset,
                                                         AFP_BaseCharacter* OwnerCharacter)
{
	if (!IsValid(WeaponDataAsset->WeaponVisualClass)) return nullptr;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = OwnerCharacter;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AFP_WeaponVisualBase* NewVisual = GetWorld()->SpawnActor<AFP_WeaponVisualBase>(
		WeaponDataAsset->WeaponVisualClass,
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		SpawnParams
	);

	if (!IsValid(NewVisual)) return nullptr;

	FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, true);
	USkeletalMeshComponent* CharMesh = OwnerCharacter->GetMesh();

	if (CharMesh && CharMesh->DoesSocketExist(FName("WeaponHoldSocket")))
	{
		NewVisual->AttachToComponent(CharMesh, AttachRules, FName("WeaponHoldSocket"));
	}

	return NewVisual;
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
