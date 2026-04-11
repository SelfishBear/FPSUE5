// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon/AnimNotify/FP_AnimNotify_ReloadFinished.h"
#include "Character/FP_PlayerCharacter.h"
#include "Component/Character/FP_PointKillAbility.h"
#include "Component/Inventory/FP_EquipmentManager.h"
#include "Weapon/WeaponLogic/FP_MeleeWeaponBase.h"
#include "Weapon/WeaponLogic/FP_WeaponBase.h"

void UFP_AnimNotify_ReloadFinished::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                           const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!IsValid(MeshComp)) return;

	AFP_PlayerCharacter* Character = Cast<AFP_PlayerCharacter>(MeshComp->GetOwner());
	if (!IsValid(Character)) return;

	UFP_EquipmentManager* EquipManager = Character->GetEquipmentManager();
	if (!IsValid(EquipManager)) return;

	UFP_WeaponBase* Weapon = EquipManager->GetCurrentWeapon();
	if (!IsValid(Weapon)) return;
	
	Character->GetPointKillAbilityComponent()->bCanPerformPointKill = true;
	EquipManager->GetCurrentMeleeWeapon()->bCanAttack = true;
	EquipManager->GetCurrentWeapon()->bCanFire = true;
	EquipManager->GetCurrentWeapon()->bCanReload = true;
	EquipManager->SetCanSwapWeapon(true);
	Weapon->FinishReload();
}

