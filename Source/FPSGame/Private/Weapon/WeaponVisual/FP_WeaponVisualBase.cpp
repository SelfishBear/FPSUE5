// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/WeaponVisual/FP_WeaponVisualBase.h"
#include "Weapon/WeaponLogic/FP_WeaponBase.h"

AFP_WeaponVisualBase::AFP_WeaponVisualBase()
{
	PrimaryActorTick.bCanEverTick = true;
	
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = WeaponMesh;
}

void AFP_WeaponVisualBase::Initialize(UFP_WeaponBase* NewWeaponLogic)
{
	WeaponLogic = NewWeaponLogic;

	// TODO: подписаться на делегаты логики (OnFired, OnReloadStarted и т.д.)
}

void AFP_WeaponVisualBase::BeginPlay()
{
	Super::BeginPlay();
}

void AFP_WeaponVisualBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
