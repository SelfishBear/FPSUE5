// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/AnimNotify/FP_AnimNotify_PointingStarted.h"

#include "Character/FP_PlayerCharacter.h"
#include "Component/Inventory/FP_EquipmentManager.h"
#include "Weapon/WeaponLogic/FP_MeleeWeaponBase.h"
#include "Weapon/WeaponVisual/FP_WeaponVisualBase.h"

class UFP_EquipmentManager;

void UFP_AnimNotify_PointingStarted::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                            const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	if (!IsValid(MeshComp)) return;

	AFP_PlayerCharacter* Character = Cast<AFP_PlayerCharacter>(MeshComp->GetOwner());
	if (!IsValid(Character)) return;

	UFP_EquipmentManager* EquipManager = Character->GetEquipmentManager();
	if (!IsValid(EquipManager)) return;

	EquipManager->GetCurrentWeaponVisual()->HideWeapon();
	EquipManager->GetCurrentMeleeWeapon()->bCanAttack = false;
	EquipManager->GetCurrentWeapon()->bCanFire = false;
	EquipManager->GetCurrentWeapon()->bCanReload = false;
	EquipManager->SetCanSwapWeapon(false);
}
