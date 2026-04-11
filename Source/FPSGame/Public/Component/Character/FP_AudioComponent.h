// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FP_AudioComponent.generated.h"

USTRUCT(BlueprintType)
struct FFootstepSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Footsteps")
	TArray<TObjectPtr<USoundBase>> WalkSounds;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Footsteps")
	TArray<TObjectPtr<USoundBase>> SprintSounds;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Footsteps")
	float WalkInterval = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Footsteps")
	float SprintInterval = 0.32f;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FPSGAME_API UFP_AudioComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFP_AudioComponent();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Footsteps")
	FFootstepSettings FootstepSettings;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TArray<TObjectPtr<USoundBase>> KillPhrases;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TArray<TObjectPtr<USoundBase>> EffortSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TArray<TObjectPtr<USoundBase>> StabSounds;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	float ChanceToPlayPhrase = 50.0f;

	void StartFootsteps(bool bSprinting);
	void StopFootsteps();
	
	UFUNCTION(BlueprintCallable, Category = "Audio")
	void PlaySound2DArray(TArray<USoundBase*> Phrases);
	
	UFUNCTION(BlueprintCallable, Category = "Audio")
	void PlaySound2DCustom(USoundBase* Phrase);
	
	UFUNCTION(BlueprintCallable, Category = "Audio")
	void PlaySoundWithChance(TArray<USoundBase*> Phrases, float Chance);

private:
	void PlayFootstepSound();
	bool RollChance(float Chance);

	FTimerHandle FootstepTimer;
	bool bCurrentSprinting = false;
};
