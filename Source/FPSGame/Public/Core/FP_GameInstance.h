// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "FP_GameInstance.generated.h"

/**
 * 
 */
UCLASS()
class FPSGAME_API UFP_GameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config")
	float DifficultyMultiplier = 1.2f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config")
	TArray<USoundBase*> FightMusicTracks;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config")
	TArray<USoundBase*> ChillMusicTracks;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config")
	TSubclassOf<UUserWidget> PauseMenuWidgetClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config")
	float LookSensitivity = 1.0f;
};
