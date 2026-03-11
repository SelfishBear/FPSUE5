// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSGame/Public/Core/FP_GameMode.h"

#include "FPSGame/Public/Character/FP_PlayerCharacter.h"
#include "FPSGame/Public/Core/FP_HUD.h"
#include "FPSGame/Public/Core/FP_PlayerController.h"

AFP_GameMode::AFP_GameMode()
{
	DefaultPawnClass = AFP_PlayerCharacter::StaticClass();
	PlayerControllerClass = AFP_PlayerController::StaticClass();
	HUDClass = AFP_HUD::StaticClass();
}
