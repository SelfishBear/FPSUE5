// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/WeaponVisual/FP_WeaponVisualBase.h"

#include "Character/FP_BaseCharacter.h"
#include "Weapon/WeaponLogic/FP_WeaponBase.h"

AFP_WeaponVisualBase::AFP_WeaponVisualBase()
{
	PrimaryActorTick.bCanEverTick = false;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = WeaponMesh;
}

void AFP_WeaponVisualBase::Initialize(UFP_WeaponBase* NewWeaponLogic)
{
	if (!IsValid(NewWeaponLogic)) return;
	
	WeaponLogic = NewWeaponLogic;
	
	WeaponLogic->OnFire.AddDynamic(this, &AFP_WeaponVisualBase::PlayFireMontage);
	WeaponLogic->OnReload.AddDynamic(this, &AFP_WeaponVisualBase::PlayReloadMontage);
}

void AFP_WeaponVisualBase::PlayFireMontage()
{
	AFP_BaseCharacter* OwnerCharacter = WeaponLogic->GetOwningCharacter();
	if (!IsValid(OwnerCharacter)) return;

	UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
	if (!IsValid(AnimInstance) || !IsValid(WeaponLogic->WeaponData->FireMontage)) return;

	AnimInstance->Montage_Play(WeaponLogic->WeaponData->FireMontage);
}

void AFP_WeaponVisualBase::PlayReloadMontage()
{
	AFP_BaseCharacter* OwnerCharacter = WeaponLogic->GetOwningCharacter();
	if (!IsValid(OwnerCharacter)) return;

	UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
	if (!IsValid(AnimInstance) || !IsValid(WeaponLogic->WeaponData->ReloadMontage)) return;

	WeaponLogic->bCanFire = false;

	AnimInstance->Montage_Play(WeaponLogic->WeaponData->ReloadMontage);
}

void AFP_WeaponVisualBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (WeaponLogic.IsValid())
	{
		WeaponLogic->OnFire.RemoveDynamic(this, &AFP_WeaponVisualBase::PlayFireMontage);
		WeaponLogic->OnReload.RemoveDynamic(this, &AFP_WeaponVisualBase::PlayReloadMontage);
	}
	
	Super::EndPlay(EndPlayReason);
}

void AFP_WeaponVisualBase::ShowWeapon()
{
	SetActorHiddenInGame(false);
}

void AFP_WeaponVisualBase::HideWeapon()
{
	SetActorHiddenInGame(true);
}
