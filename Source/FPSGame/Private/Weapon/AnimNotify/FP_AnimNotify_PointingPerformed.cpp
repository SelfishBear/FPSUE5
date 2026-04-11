// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/AnimNotify/FP_AnimNotify_PointingPerformed.h"

#include "Character/FP_PlayerCharacter.h"
#include "Component/Character/FP_PointKillAbility.h"

void UFP_AnimNotify_PointingPerformed::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                              const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
		
	if (!IsValid(MeshComp)) return;

	AFP_PlayerCharacter* Character = Cast<AFP_PlayerCharacter>(MeshComp->GetOwner());
	if (!IsValid(Character)) return;

	UFP_PointKillAbility* PointKillAbility = Character->GetPointKillAbilityComponent();
	if (!IsValid(PointKillAbility)) return;
	
	PointKillAbility->PerformPointKill();
}
