// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon/AnimNotify/FP_AnimNotify_FireStarted.h"
#include "Character/FP_BaseCharacter.h"
#include "Character/FP_PlayerCharacter.h"
#include "Component/Inventory/FP_EquipmentManager.h"
#include "Weapon/WeaponLogic/FP_WeaponBase.h"
#include "Weapon/WeaponVisual/FP_WeaponVisualBase.h"

void UFP_AnimNotify_FireStarted::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
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
	
	AFP_WeaponVisualBase* WeaponVisual = EquipManager->GetCurrentWeaponVisual();
	if (!IsValid(WeaponVisual)) return;
	
	Weapon->PerformFireLogic();
	WeaponVisual->PerformFeedback();
}

