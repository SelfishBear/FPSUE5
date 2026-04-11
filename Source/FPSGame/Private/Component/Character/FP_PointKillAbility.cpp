// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/Character/FP_PointKillAbility.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Character/FP_PlayerCharacter.h"

UFP_PointKillAbility::UFP_PointKillAbility()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UFP_PointKillAbility::StartPointKill()
{
	if (!bCanPerformPointKill) return;

	AFP_PlayerCharacter* PlayerCharacter = Cast<AFP_PlayerCharacter>(GetOwner());
	if (!IsValid(PlayerCharacter)) return;

	UAnimInstance* AnimInstance = PlayerCharacter->GetMesh()->GetAnimInstance();
	if (!IsValid(AnimInstance) || !IsValid(PointKillMontage)) return;

	AnimInstance->Montage_Play(PointKillMontage);

	bCanPerformPointKill = false;
	GetWorld()->GetTimerManager().SetTimer(
		FP_PointKillTimerHandle,
		[this]() { bCanPerformPointKill = true; },
		PointKillCooldown,
		false
	);
}

void UFP_PointKillAbility::PerformPointKill()
{
	AFP_PlayerCharacter* PlayerCharacter = Cast<AFP_PlayerCharacter>(GetOwner());
	if (!IsValid(PlayerCharacter)) return;

	APlayerController* PlayerController = Cast<APlayerController>(PlayerCharacter->GetController());
	if (!IsValid(PlayerController)) return;

	FVector TraceStart;
	FRotator CameraRotation;
	PlayerController->GetPlayerViewPoint(TraceStart, CameraRotation);

	const FVector TraceEnd = TraceStart + CameraRotation.Vector() * 10000.0f;

	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(PlayerCharacter);
	CollisionQueryParams.bTraceComplex = true;

	FHitResult HitResult;
	const bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility,
	                                                       CollisionQueryParams);

#if ENABLE_DRAW_DEBUG
	DrawDebugLine(GetWorld(), TraceStart, bHit ? HitResult.ImpactPoint : TraceEnd,
	              bHit ? FColor::Red : FColor::Green, false, 1.0f, 0, 1.0f);
#endif

	if (HitResult.GetActor()->GetClass()->ImplementsInterface(UFP_Damageable::StaticClass()))
	{
		IFP_Damageable::Execute_ReceiveDamage(HitResult.GetActor(), PointKillDamage);
	}

	if (!IsValid(PointKillFX)) return;

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), PointKillFX, HitResult.ImpactPoint,
	                                               FRotator::ZeroRotator);
}
