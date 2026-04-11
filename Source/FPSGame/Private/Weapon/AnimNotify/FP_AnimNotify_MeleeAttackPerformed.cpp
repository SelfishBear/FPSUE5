// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/AnimNotify/FP_AnimNotify_MeleeAttackPerformed.h"

#include "Character/FP_PlayerCharacter.h"
#include "Component/Character/FP_AudioComponent.h"
#include "Component/Inventory/FP_EquipmentManager.h"
#include "Weapon/WeaponLogic/FP_MeleeWeaponBase.h"

class AFP_PlayerCharacter;

void UFP_AnimNotify_MeleeAttackPerformed::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                 const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Melee Attack Performed"));
	
	AFP_PlayerCharacter* Character = Cast<AFP_PlayerCharacter>(MeshComp->GetOwner());
	if (!IsValid(Character)) return;

	UFP_EquipmentManager* EquipManager = Character->GetEquipmentManager();
	if (!IsValid(EquipManager)) return;
	
	UFP_AudioComponent* AudioComponent = Character->GetFP_AudioComponent();
	if (!IsValid(AudioComponent)) return;
	
	EquipManager->GetCurrentMeleeWeapon()->MakeAttack();
	AudioComponent->PlaySound2DArray(AudioComponent->StabSounds);
}
