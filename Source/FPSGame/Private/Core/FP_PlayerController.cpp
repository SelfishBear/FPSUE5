// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSGame/Public/Core/FP_PlayerController.h"
#include "EnhancedInputSubsystems.h"

AFP_PlayerController::AFP_PlayerController()
{
}

void AFP_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, DefaultMappingPriority);
	}
}
