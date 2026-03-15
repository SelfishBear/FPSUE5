// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/FP_BaseEnemy.h"


// Sets default values
AFP_BaseEnemy::AFP_BaseEnemy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AFP_BaseEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFP_BaseEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AFP_BaseEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

