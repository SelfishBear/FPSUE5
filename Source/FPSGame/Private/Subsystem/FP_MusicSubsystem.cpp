// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/FP_MusicSubsystem.h"

#include "Components/AudioComponent.h"
#include "Core/FP_GameInstance.h"
#include "Kismet/GameplayStatics.h"

void UFP_MusicSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	InitAudioComponent();
	LoadMusicTracks();
}

void UFP_MusicSubsystem::InitAudioComponent()
{
	UFP_GameInstance* GameInstance = Cast<UFP_GameInstance>(GetGameInstance());
	if (IsValid(GameInstance))
	{
		AudioComponent = NewObject<UAudioComponent>(GameInstance);
		AudioComponent->RegisterComponent();

		AudioComponent->bIsUISound = true;
		AudioComponent->bAutoDestroy = false;
		AudioComponent->bStopWhenOwnerDestroyed = true;
	}
}

void UFP_MusicSubsystem::PlayFightMusic()
{
	if (FightMusicTracks.Num() == 0) return;

	USoundBase* MusicTrack = GetRandomMusicTrack(FightMusicTracks);
	if (!IsValid(MusicTrack)) return;

	StopCurrentMusic();

	UWorld* World = GetGameInstance()->GetWorld();
	if (!IsValid(World)) return;

	AudioComponent = UGameplayStatics::SpawnSound2D(
		World,
		MusicTrack,
		1.0f,   // VolumeMultiplier
		1.0f,   // PitchMultiplier
		0.0f,   // StartTime
		nullptr,
		false,  // bPersistAcrossLevelTransitions
		true    // bAutoDestroy = false чтобы управлять вручную
	);
}

void UFP_MusicSubsystem::PlayChillMusic()
{
	if (ChillMusicTracks.Num() == 0) return;

	USoundBase* MusicTrack = GetRandomMusicTrack(ChillMusicTracks);
	if (!IsValid(MusicTrack)) return;

	StopCurrentMusic();

	UWorld* World = GetGameInstance()->GetWorld();
	if (!IsValid(World)) return;

	AudioComponent = UGameplayStatics::SpawnSound2D(
		World,
		MusicTrack,
		1.0f,
		1.0f,
		0.0f,
		nullptr,
		false,
		true
	);
}

void UFP_MusicSubsystem::LoadMusicTracks()
{
	UFP_GameInstance* GameInstance = Cast<UFP_GameInstance>(GetGameInstance());
	if (!IsValid(GameInstance)) return;

	FightMusicTracks = GameInstance->FightMusicTracks;
	ChillMusicTracks = GameInstance->ChillMusicTracks;
}

USoundBase* UFP_MusicSubsystem::GetRandomMusicTrack(const TArray<USoundBase*>& MusicTracks) const
{
	int32 RandomIndex = FMath::RandRange(0, MusicTracks.Num() - 1);
	return MusicTracks.IsValidIndex(RandomIndex) ? MusicTracks[RandomIndex] : nullptr;
}

void UFP_MusicSubsystem::StopCurrentMusic()
{
	if (IsValid(AudioComponent) && AudioComponent->IsPlaying())
	{
		AudioComponent->FadeOut(1.5f, 0.0f, EAudioFaderCurve::Linear);
		AudioComponent = nullptr;
	}
}
