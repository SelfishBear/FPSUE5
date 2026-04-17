// Fill out your copyright notice in the Description page of Project Settings.

#include "Component/Character/FP_AudioComponent.h"
#include "Kismet/GameplayStatics.h"

UFP_AudioComponent::UFP_AudioComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UFP_AudioComponent::StartFootsteps(const bool bSprinting)
{
	const float Interval = bSprinting ? FootstepSettings.SprintInterval : FootstepSettings.WalkInterval;

	if (bCurrentSprinting == bSprinting && GetWorld()->GetTimerManager().IsTimerActive(FootstepTimer)) return;

	bCurrentSprinting = bSprinting;
	GetWorld()->GetTimerManager().SetTimer(FootstepTimer, this, &UFP_AudioComponent::PlayFootstepSound, Interval, true,
	                                       0.0f);
}

void UFP_AudioComponent::StopFootsteps()
{
	GetWorld()->GetTimerManager().ClearTimer(FootstepTimer);
}

void UFP_AudioComponent::PlayFootstepSound()
{
	const TArray<TObjectPtr<USoundBase>>& Sounds = bCurrentSprinting
		                                               ? FootstepSettings.SprintSounds
		                                               : FootstepSettings.WalkSounds;
	if (Sounds.Num() == 0) return;

	USoundBase* Sound = Sounds[FMath::RandRange(0, Sounds.Num() - 1)].Get();
	if (!IsValid(Sound)) return;
	UGameplayStatics::PlaySound2D(GetWorld(), Sound);
}

void UFP_AudioComponent::PlaySound2DArray(TArray<USoundBase*> Phrases)
{
	if (Phrases.Num() == 0) return;

	USoundBase* SelectedPhrase = Phrases[FMath::RandRange(0, Phrases.Num() - 1)];
	if (!IsValid(SelectedPhrase)) return;
	UGameplayStatics::PlaySound2D(GetWorld(), SelectedPhrase);
}

void UFP_AudioComponent::PlaySound2DCustom(USoundBase* Phrase)
{
	if (!IsValid(Phrase)) return;

	UGameplayStatics::PlaySound2D(GetWorld(), Phrase);
}

void UFP_AudioComponent::PlaySoundWithChance(TArray<USoundBase*> Phrases, float Chance)
{
	if (!RollChance(Chance) || Phrases.Num() == 0) return;

	USoundBase* Sound = Phrases[FMath::RandRange(0, Phrases.Num() - 1)];
	if (!IsValid(Sound)) return;
	UGameplayStatics::PlaySound2D(GetWorld(), Sound);
}

bool UFP_AudioComponent::RollChance(float Chance)
{
	return FMath::FRand() < (Chance / 100.0f);
}
