#include "FP_DebugHelper.h"

#include "Character/Zombie/FP_BaseZombie.h"

static FAutoConsoleCommand GetZombiesMultipliersCommand
(
	TEXT("FP.Debug.ResetZombiesMultipliers"),
	TEXT("Reset zombies multipliers to default values"),
	FConsoleCommandDelegate::CreateStatic(&FP_DebugHelper::ResetZombiesMultiplier)
);

void FP_DebugHelper::ResetZombiesMultiplier()
{
	AFP_BaseZombie::ResetMultipliers();
}
