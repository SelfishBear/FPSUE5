// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/WeaponVisual/FP_MeleeWeaponVisualBase.h"

#include "Character/FP_BaseCharacter.h"
#include "DataAsset/Weapon/FP_MeleeWeaponDataAsset.h"
#include "Weapon/WeaponLogic/FP_MeleeWeaponBase.h"

AFP_MeleeWeaponVisualBase::AFP_MeleeWeaponVisualBase()
{
	PrimaryActorTick.bCanEverTick = false;

	MeleeWeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeleeWeaponMesh"));
	RootComponent = MeleeWeaponMesh;
}

void AFP_MeleeWeaponVisualBase::BeginPlay()
{
	Super::BeginPlay();

	HideMelee();
}

void AFP_MeleeWeaponVisualBase::Initialize(UFP_MeleeWeaponBase* NewWeaponLogic)
{
	if (!IsValid(NewWeaponLogic)) return;

	MeleeLogic = NewWeaponLogic;

	MeleeLogic->OnAttack.AddDynamic(this, &AFP_MeleeWeaponVisualBase::AttackAction);
}

void AFP_MeleeWeaponVisualBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (MeleeLogic.IsValid())
	{
		MeleeLogic->OnAttack.RemoveDynamic(this, &AFP_MeleeWeaponVisualBase::AttackAction);
	}
	
	Super::EndPlay(EndPlayReason);
}

void AFP_MeleeWeaponVisualBase::PlayAttackMontage()
{
	AFP_BaseCharacter* OwnerCharacter = MeleeLogic->GetOwningCharacter();
	if (!IsValid(OwnerCharacter)) return;

	UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
	if (!IsValid(AnimInstance) || !IsValid(MeleeLogic->MeleeWeaponData->AttackMontage)) return;

	AnimInstance->Montage_Play(MeleeLogic->MeleeWeaponData->AttackMontage);
}

void AFP_MeleeWeaponVisualBase::AttackAction()
{
	PlayAttackMontage();
}

void AFP_MeleeWeaponVisualBase::ShowMelee()
{
	SetActorHiddenInGame(false);
}

void AFP_MeleeWeaponVisualBase::HideMelee()
{
	SetActorHiddenInGame(true);
}
