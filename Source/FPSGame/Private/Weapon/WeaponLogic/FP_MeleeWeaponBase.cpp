// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/WeaponLogic/FP_MeleeWeaponBase.h"

#include "Character/FP_BaseCharacter.h"
#include "Component/Character/FP_CharacterMovementComponent.h"

void UFP_MeleeWeaponBase::PerformAttack()
{
	if (!CanPerformAttack()) return;
	OnAttack.Broadcast();
}

void UFP_MeleeWeaponBase::MakeAttack()
{
	FHitResult HitResult;
	const bool bHit = MakeTrace(HitResult);
}

bool UFP_MeleeWeaponBase::MakeTrace(FHitResult& OutHitResult) const
{
	AFP_BaseCharacter* Character = OwningCharacter.Get();
	if (!IsValid(Character)) return false;
	
	const APlayerController* PC = Cast<APlayerController>(Character->GetController());
	if (!IsValid(PC)) return false;

	FVector TraceStart;
	FRotator CameraRotation;
	PC->GetPlayerViewPoint(TraceStart, CameraRotation);
	
	FVector ShotDirection = CameraRotation.Vector();
	
	const FVector TraceEnd = TraceStart + ShotDirection * AttackRange;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Character);
	QueryParams.bTraceComplex = true;

	const bool bHit = GetWorld()->LineTraceSingleByChannel(
		OutHitResult,
		TraceStart,
		TraceEnd,
		ECC_Visibility,
		QueryParams
	);

#if ENABLE_DRAW_DEBUG
	DrawDebugLine(GetWorld(), TraceStart, bHit ? OutHitResult.ImpactPoint : TraceEnd,
				  bHit ? FColor::Red : FColor::Green, false, 1.0f, 0, 1.0f);
#endif

	return bHit;
}
