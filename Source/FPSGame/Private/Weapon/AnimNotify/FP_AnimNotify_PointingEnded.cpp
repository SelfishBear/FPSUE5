// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/AnimNotify/FP_AnimNotify_PointingEnded.h"

#include "Character/FP_PlayerCharacter.h"
#include "Component/Inventory/FP_EquipmentManager.h"
#include "Weapon/WeaponLogic/FP_MeleeWeaponBase.h"
#include "Weapon/WeaponVisual/FP_WeaponVisualBase.h"

class AFP_PlayerCharacter;
class UFP_EquipmentManager;

void UFP_AnimNotify_PointingEnded::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                          const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
		
	if (!IsValid(MeshComp)) return;

	AFP_PlayerCharacter* Character = Cast<AFP_PlayerCharacter>(MeshComp->GetOwner());
	if (!IsValid(Character)) return;

	UFP_EquipmentManager* EquipManager = Character->GetEquipmentManager();
	if (!IsValid(EquipManager)) return;

	EquipManager->GetCurrentWeaponVisual()->ShowWeapon();
	EquipManager->GetCurrentMeleeWeapon()->bCanAttack = true;
	EquipManager->GetCurrentWeapon()->bCanFire = true;
	EquipManager->GetCurrentWeapon()->bCanReload = true;
	EquipManager->SetCanSwapWeapon(true);
}
