// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/WeaponLogic/FP_WeaponBase.h"

#include "DataAsset/Weapon/FP_WeaponDataAsset.h"

#include "Character/FP_BaseCharacter.h"
#include "Engine/Engine.h"
#include "DrawDebugHelpers.h"
#include "Character/FP_PlayerCharacter.h"
#include "Component/Character/FP_CharacterMovementComponent.h"
#include "Component/Inventory/FP_EquipmentManager.h"

UWorld* UFP_WeaponBase::GetWorld() const
{
	if (const AFP_BaseCharacter* Owner = OwningCharacter.Get())
	{
		return Owner->GetWorld();
	}
	return nullptr;
}

void UFP_WeaponBase::StartFire()
{
	if (!CanFire() || !IsValid(WeaponData)) return;

	bIsFiring = true;
	CurrentState = EWeaponState::Firing;

	const float FireInterval = 60.0f / WeaponData->FireRate;

	HandleFireTimer();

	switch (CurrentFireMode)
	{
	case EFireMode::SemiAuto: break;

	case EFireMode::Burst:
		BurstShotsRemaining = 2;
		GetWorld()->GetTimerManager().SetTimer(
			FireTimerHandle,
			this,
			&UFP_WeaponBase::HandleFireTimer,
			FireInterval,
			true
		);
		break;

	case EFireMode::FullAuto:
		GetWorld()->GetTimerManager().SetTimer(
			FireTimerHandle,
			this,
			&UFP_WeaponBase::HandleFireTimer,
			FireInterval,
			true
		);
		break;
	}
}

void UFP_WeaponBase::StopFire()
{
	bIsFiring = false;

	if (!IsValid(GetWorld())) return;

	GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);


	if (CurrentState == EWeaponState::Firing)
	{
		CurrentState = EWeaponState::Idle;
	}
}

void UFP_WeaponBase::Reload()
{
	if (CurrentState == EWeaponState::Reloading || CurrentAmmo >= WeaponData->MaxAmmo || CurrentReserveAmmo <= 0)
		return;

	CurrentState = EWeaponState::Reloading;

	OnReload.Broadcast();
}

void UFP_WeaponBase::FinishReload()
{
	if (CurrentState != EWeaponState::Reloading || !IsValid(WeaponData)) return;

	const float NeededAmmo = WeaponData->MaxAmmo - CurrentAmmo;
	const float AmmoToAdd = FMath::Min(NeededAmmo, CurrentReserveAmmo);
	CurrentReserveAmmo -= AmmoToAdd;
	CurrentAmmo += AmmoToAdd;
	CurrentState = EWeaponState::Idle;
	bCanFire = true;

	BroadcastAmmoChanged();
}

void UFP_WeaponBase::ConsumeAmmo()
{
	if (CurrentAmmo > 0)
	{
		CurrentAmmo--;
		BroadcastAmmoChanged();
	}
}

void UFP_WeaponBase::PrintDebugMessage(FHitResult HitResult, const bool bHit)
{
	if (bHit && HitResult.GetActor())
	{
		const FString HitActorName = HitResult.GetActor()->GetName();
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow,
			                                 FString::Printf(TEXT("Hit: %s"), *HitActorName));
		}
	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, TEXT("Hit: Nothing"));
		}
	}
}

bool UFP_WeaponBase::MakeTrace(FHitResult& OutHitResult) const
{
	AFP_BaseCharacter* Character = OwningCharacter.Get();
	if (!IsValid(Character)) return false;

	UFP_CharacterMovementComponent* FP_CharacterMovementComponent = Character->GetFP_MovementComponent();
	if (!IsValid(FP_CharacterMovementComponent)) return false;

	const APlayerController* PC = Cast<APlayerController>(Character->GetController());
	if (!IsValid(PC)) return false;

	FVector TraceStart;
	FRotator CameraRotation;
	PC->GetPlayerViewPoint(TraceStart, CameraRotation);

	const float HalfAngleRad = FMath::DegreesToRadians(WeaponData->SpreadAngle);

	FVector ShotDirection = (FP_CharacterMovementComponent->Velocity != FVector::ZeroVector)
		                        ? FMath::VRandCone(CameraRotation.Vector(), HalfAngleRad)
		                        : CameraRotation.Vector();
	
	const FVector TraceEnd = TraceStart + ShotDirection * TraceDistance;

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

void UFP_WeaponBase::BroadcastAmmoChanged()
{
	AFP_PlayerCharacter* FP_PlayerCharacter = Cast<AFP_PlayerCharacter>(GetOwningCharacter());
	if (IsValid(FP_PlayerCharacter))
	{
		FP_PlayerCharacter->GetEquipmentManager()->OnWeaponAmmoChanged.Broadcast(this);
	}
}

void UFP_WeaponBase::PerformFire()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("Performing Fire Logic"));
	if (!IsValid(WeaponData)) return;

	UE_LOG(LogTemp, Warning, TEXT("OnFire.Broadcast, IsBound: %s"), OnFire.IsBound() ? TEXT("true") : TEXT("false"));

	OnFire.Broadcast();
}

void UFP_WeaponBase::PerformFireLogic()
{
	if (!IsValid(WeaponData)) return;

	FHitResult HitResult;
	const bool bHit = MakeTrace(HitResult);

	ConsumeAmmo();
	PrintDebugMessage(HitResult, bHit);
}

void UFP_WeaponBase::HandleFireTimer()
{
	if (!CanFire())
	{
		StopFire();
		return;
	}

	PerformFire();

	switch (CurrentFireMode)
	{
	case EFireMode::SemiAuto: StopFire();
		break;

	case EFireMode::Burst:
		BurstShotsRemaining--;
		if (BurstShotsRemaining <= 0) StopFire();
		break;

	case EFireMode::FullAuto: break;
	}
}
