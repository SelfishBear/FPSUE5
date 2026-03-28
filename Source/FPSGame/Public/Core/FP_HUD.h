// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "FP_HUD.generated.h"

/**
 * 
 */
UCLASS()
class FPSGAME_API AFP_HUD : public AHUD
{
	GENERATED_BODY()

public:
	AFP_HUD();

	UFUNCTION(BlueprintImplementableEvent, Category="Shop")
	void CreateHUDWidget();

protected:
	virtual void BeginPlay() override;
};
