// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Zombie/FP_BaseZombie.h"

#include "Component/Character/FP_CharacterMovementComponent.h"
#include "Component/Character/FP_HealthComponent.h"
#include "Components/StateTreeComponent.h"
#include "DataAsset/Zombie/FP_ZombieConfig.h"

AFP_BaseZombie::AFP_BaseZombie(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
}

void AFP_BaseZombie::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	InitComponents();
}

void AFP_BaseZombie::BeginPlay()
{
	Super::BeginPlay();

	InitStats();
}

void AFP_BaseZombie::InitComponents()
{
	if (!IsValid(ZombieDataAsset)) return;
	FP_HealthComponent->MaxHealth = ZombieDataAsset->MaxHealth * HealthMultiplier;
	FP_MovementComponent->MaxWalkSpeed = ZombieDataAsset->MoveSpeed * SpeedMultiplier;
}

void AFP_BaseZombie::InitStats()
{
	if (!IsValid(ZombieDataAsset)) return;

	CurrentAttackTraceDistance = ZombieDataAsset->AttackTraceDistance;
	CurrentAttackRadius = ZombieDataAsset->AttackRadius;
	CurrentAttackDamage = ZombieDataAsset->AttackDamage * DamageMultiplier;
}

void AFP_BaseZombie::ReturnToBaseSpeed()
{
	UFP_CharacterMovementComponent* MovementComponent = GetFP_MovementComponent();
	if (!IsValid(MovementComponent)) return;
	
	MovementComponent->MaxWalkSpeed = (CurrentMoveSpeed == 0.0f) ? ZombieDataAsset->MoveSpeed * SpeedMultiplier : CurrentMoveSpeed;
}

void AFP_BaseZombie::SlowDown()
{
	UFP_CharacterMovementComponent* MovementComponent = GetFP_MovementComponent();
	if (!IsValid(MovementComponent)) return;
	
	MovementComponent->MaxWalkSpeed *= 0.5f;
}
