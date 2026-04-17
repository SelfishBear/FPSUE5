// Fill out your copyright notice in the Description page of Project Settings.


#include "Spawner/FP_ZombieSpawner.h"

#include "Character/FP_PlayerCharacter.h"
#include "Component/Character/FP_HealthComponent.h"
#include "Core/FP_GameInstance.h"
#include "Core/FP_GameMode.h"
#include "Kismet/GameplayStatics.h"

AFP_ZombieSpawner::AFP_ZombieSpawner()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AFP_ZombieSpawner::SpawnZombies()
{
	if (SpawnPoints.IsEmpty() || AvailableZombieTypes.IsEmpty()) return;

	int32 Remaining = CurrentAmountToSpawn;
	int32 TypeIndex = FMath::RandRange(0, AvailableZombieTypes.Num() - 1);
	int32 AttemptCount = 0;
	const int32 MaxAttempts = AvailableZombieTypes.Num();

	while (Remaining > 0)
	{
		if (AttemptCount >= MaxAttempts)
		{
			UE_LOG(LogTemp, Warning, TEXT("ZombieSpawner: no valid classes"));
			break;
		}

		const EZombieType Type = AvailableZombieTypes[TypeIndex % AvailableZombieTypes.Num()];
		const TSubclassOf<AFP_BaseZombie>* ZombieClass = ZombieClasses.Find(Type);

		if (!ZombieClass || !*ZombieClass)
		{
			TypeIndex++;
			AttemptCount++;
			continue;
		}

		AttemptCount = 0;

		const FVector SpawnLocation = SpawnPoints[FMath::RandRange(0, SpawnPoints.Num() - 1)];

		AFP_BaseZombie* Zombie = GetWorld()->SpawnActor<AFP_BaseZombie>(
			*ZombieClass, SpawnLocation, FRotator::ZeroRotator
		);

		if (IsValid(Zombie))
		{
			Remaining--;
			ZombiesLeft++;
			Zombie->GetFP_HealthComponent()->OnDeath.AddDynamic(this, &AFP_ZombieSpawner::OnKilledZombie);
		}
		TypeIndex++;
	}
	OnZombiesSpawned.Broadcast(ZombiesLeft);
}

void AFP_ZombieSpawner::OnKilledZombie()
{
	ZombiesLeft--;
	UE_LOG(LogTemp, Warning, TEXT("[ZombieSpawner] OnKilledZombie called. ZombiesLeft = %d"), ZombiesLeft);
	OnZombieDead.Broadcast(ZombiesLeft);
}

void AFP_ZombieSpawner::UpgradeZombies()
{
	UpgradeZombiesAmount();
	UpgradeZombiesStats();
	UpgradeZombiesType();
}

void AFP_ZombieSpawner::UpgradeZombiesType()
{
	TArray<EZombieType> LockedZombies;

	for (const EZombieType Type : AllZombieTypes)
	{
		if (!AvailableZombieTypes.Contains(Type))
		{
			LockedZombies.AddUnique(Type);
		}
	}

	UE_LOG(LogTemp, Warning,
	       TEXT(
		       "[ZombieSpawner] UpgradeZombiesType called. LockedZombies.Num() = %d, AvailableZombieTypes.Num() before = %d"
	       ), LockedZombies.Num(), AvailableZombieTypes.Num());

	if (LockedZombies.IsEmpty()) return;

	const EZombieType TypeToUnlock = LockedZombies[FMath::RandRange(0, LockedZombies.Num() - 1)];
	AvailableZombieTypes.AddUnique(TypeToUnlock);

	UE_LOG(LogTemp, Warning, TEXT("[ZombieSpawner] UpgradeZombiesType done. AvailableZombieTypes.Num() after = %d"),
	       AvailableZombieTypes.Num());
}

void AFP_ZombieSpawner::UpgradeZombiesAmount()
{
	if (!GetWorld()) return;
	UFP_GameInstance* FP_GameInstance = Cast<UFP_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (!FP_GameInstance) return;

	CurrentAmountToSpawn = CurrentAmountToSpawn + (2 * FP_GameInstance->DifficultyMultiplier);
}

void AFP_ZombieSpawner::UpgradeZombiesStats()
{
	if (!GetWorld()) return;
	UFP_GameInstance* FP_GameInstance = Cast<UFP_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (!FP_GameInstance) return;

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow,
	                                 FString::Printf(
		                                 TEXT("Upgrading zombies stats with multiplier: %f"),
		                                 FP_GameInstance->DifficultyMultiplier));

	AFP_BaseZombie::HealthMultiplier *= FP_GameInstance->DifficultyMultiplier;
	AFP_BaseZombie::SpeedMultiplier *= FP_GameInstance->DifficultyMultiplier;
	AFP_BaseZombie::DamageMultiplier *= FP_GameInstance->DifficultyMultiplier;

	UE_LOG(LogTemp, Warning, TEXT("[ZombieSpawner] UpgradeZombiesStats done. New multipliers - Health: %f, Speed: %f, Damage: %f"), AFP_BaseZombie::HealthMultiplier, AFP_BaseZombie::SpeedMultiplier, AFP_BaseZombie::DamageMultiplier);
	
	
}
