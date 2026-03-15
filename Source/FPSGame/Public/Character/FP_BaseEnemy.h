// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FP_BaseCharacter.h"
#include "FP_BaseEnemy.generated.h"

UCLASS()
class FPSGAME_API AFP_BaseEnemy : public AFP_BaseCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFP_BaseEnemy(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
