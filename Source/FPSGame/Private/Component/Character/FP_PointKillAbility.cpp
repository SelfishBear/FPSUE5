// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/Character/FP_PointKillAbility.h"
#include "NiagaraFunctionLibrary.h"
#include "Character/FP_PlayerCharacter.h"
#include "Engine/OverlapResult.h"

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

	TArray<FOverlapResult> OverlapResults;
	FHitResult HitResult;

	const bool bHitTrace = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		TraceStart,
		TraceEnd,
		ECC_Visibility,
		CollisionQueryParams
	);

	const bool bHitSphere = GetWorld()->OverlapMultiByChannel(
		OverlapResults,
		bHitTrace ? HitResult.ImpactPoint : TraceEnd,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(Radius),
		CollisionQueryParams
	);

#if ENABLE_DRAW_DEBUG
	DrawDebugLine(GetWorld(), TraceStart, bHitTrace ? HitResult.ImpactPoint : TraceEnd,
	              bHitTrace ? FColor::Red : FColor::Green, false, 1.0f, 0, 1.0f);
	DrawDebugSphere(GetWorld(), bHitTrace ? HitResult.ImpactPoint : TraceEnd, Radius, 12,
	                bHitSphere ? FColor::Red : FColor::Green, false, 1.0f);
#endif

	if (!bHitTrace) return;
	SpawnVFX(HitResult);
	
	if (!bHitSphere) return;

	ApplyDamage(OverlapResults);
}

void UFP_PointKillAbility::ApplyDamage(const TArray<FOverlapResult>& Overlaps)
{
	for (const FOverlapResult& Overlap : Overlaps)
	{
		AActor* HitActor = Overlap.GetActor();
		if (!IsValid(HitActor)) continue;
		if (!HitActor->GetClass()->ImplementsInterface(UFP_Damageable::StaticClass())) continue;

		IFP_Damageable::Execute_ReceiveDamage(HitActor, PointKillDamage);
	}
}

void UFP_PointKillAbility::SpawnVFX(const FHitResult& Hit)
{
	if (!IsValid(PointKillFX)) return;

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), PointKillFX, Hit.ImpactPoint,
	                                               FRotator::ZeroRotator);
}
