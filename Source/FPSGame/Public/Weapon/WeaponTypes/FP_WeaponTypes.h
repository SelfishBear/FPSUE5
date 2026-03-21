#pragma once

#include "CoreMinimal.h"
#include "FP_WeaponTypes.generated.h"

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	Idle,
	Firing,
	Reloading,
};

UENUM(BlueprintType)
enum class EFireMode : uint8
{
	SemiAuto,
	Burst,
	FullAuto,
};

UENUM(BlueprintType)
enum class EWeaponSlot : uint8
{
	Primary,
	Secondary,
};

