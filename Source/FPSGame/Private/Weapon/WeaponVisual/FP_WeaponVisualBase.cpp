// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/WeaponVisual/FP_WeaponVisualBase.h"

#include "Character/FP_BaseCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Weapon/WeaponLogic/FP_WeaponBase.h"

AFP_WeaponVisualBase::AFP_WeaponVisualBase()
{
	PrimaryActorTick.bCanEverTick = false;
	
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = WeaponMesh;
	
	FirePosition = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FirePosition"));
	FirePosition->SetupAttachment(WeaponMesh);
}

void AFP_WeaponVisualBase::Initialize(UFP_WeaponBase* NewWeaponLogic)
{
	if (!IsValid(NewWeaponLogic)) return;

	WeaponLogic = NewWeaponLogic;

	WeaponLogic->OnFire.AddDynamic(this, &AFP_WeaponVisualBase::PlayFireMontage);
	WeaponLogic->OnReload.AddDynamic(this, &AFP_WeaponVisualBase::PlayReloadMontage);
}

void AFP_WeaponVisualBase::PlayFireMontage()
{
	AFP_BaseCharacter* OwnerCharacter = WeaponLogic->GetOwningCharacter();
	if (!IsValid(OwnerCharacter)) return;

	UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
	if (!IsValid(AnimInstance) || !IsValid(WeaponLogic->WeaponData->FireMontage)) return;

	AnimInstance->Montage_Play(WeaponLogic->WeaponData->FireMontage);
}

void AFP_WeaponVisualBase::PlayReloadMontage()
{
	AFP_BaseCharacter* OwnerCharacter = WeaponLogic->GetOwningCharacter();
	if (!IsValid(OwnerCharacter)) return;

	UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
	if (!IsValid(AnimInstance) || !IsValid(WeaponLogic->WeaponData->ReloadMontage)) return;

	WeaponLogic->bCanFire = false;

	AnimInstance->Montage_Play(WeaponLogic->WeaponData->ReloadMontage, WeaponLogic->CurrentReloadTime);
}
void AFP_WeaponVisualBase::SpawnVFX_Implementation()
{
	UGameplayStatics::SpawnEmitterAttached(
	WeaponLogic->WeaponData->MuzzleFlashFX,
	FirePosition,
	TEXT("FireSocket"),
	FVector::ZeroVector,
	FRotator::ZeroRotator,
	FVector(FireFXScale),
	EAttachLocation::SnapToTarget,
	true
);
}

void AFP_WeaponVisualBase::PerformFeedback()
{
	if (!WeaponLogic->WeaponData->MuzzleFlashFX) return;
	if (!WeaponLogic->WeaponData->FireSound) return;
	

	SpawnVFX();
	
	UGameplayStatics::PlaySound2D(GetWorld(), WeaponLogic->WeaponData->FireSound);
}

void AFP_WeaponVisualBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (WeaponLogic.IsValid())
	{
		WeaponLogic->OnFire.RemoveDynamic(this, &AFP_WeaponVisualBase::PlayFireMontage);
		WeaponLogic->OnReload.RemoveDynamic(this, &AFP_WeaponVisualBase::PlayReloadMontage);
	}

	Super::EndPlay(EndPlayReason);
}

void AFP_WeaponVisualBase::ShowWeapon()
{
	SetActorHiddenInGame(false);
}

void AFP_WeaponVisualBase::HideWeapon()
{
	SetActorHiddenInGame(true);
}

void AFP_WeaponVisualBase::PlayShake()
{
	APlayerController* PC = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());
	if (!IsValid(PC) || !IsValid(CameraShake)) return;

	PC->ClientStartCameraShake(CameraShake);
}
