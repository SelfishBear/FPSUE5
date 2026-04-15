// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "FP_MusicSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class FPSGAME_API UFP_MusicSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category="Music")
	void PlayFightMusic();
	
	UFUNCTION(BlueprintCallable, Category="Music")
	void PlayChillMusic();
	
	UFUNCTION(BlueprintCallable, Category="Music")
	void StopCurrentMusic();

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	void InitAudioComponent();
	
	UPROPERTY()
	UAudioComponent* AudioComponent;

	UPROPERTY()
	TArray<USoundBase*> FightMusicTracks;
	
	UPROPERTY()
	TArray<USoundBase*> ChillMusicTracks;

	void LoadMusicTracks();
	
	USoundBase* GetRandomMusicTrack(const TArray<USoundBase*>& MusicTracks) const;
};
