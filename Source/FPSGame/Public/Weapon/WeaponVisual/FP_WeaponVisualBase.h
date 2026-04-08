// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FP_WeaponVisualBase.generated.h"

class UFP_WeaponBase;

UCLASS()
class FPSGAME_API AFP_WeaponVisualBase : public AActor
{
	GENERATED_BODY()

public:
	AFP_WeaponVisualBase();
	
	void Initialize(UFP_WeaponBase* NewWeaponLogic);

	void ShowWeapon();

	void HideWeapon();
	
	void PlayShake();

	UFUNCTION()
	void PlayFireMontage();

	UFUNCTION()
	void PlayReloadMontage();

	UFUNCTION(BLueprintImplementableEvent, Category = "Weapon|Visual")
	void PerformFeedback();

	UFUNCTION(BlueprintPure, Category = "Weapon")
	FORCEINLINE UFP_WeaponBase* GetWeaponLogic() const { return WeaponLogic.Get(); }

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> WeaponMesh;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TSubclassOf<UCameraShakeBase> CameraShake;

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	TWeakObjectPtr<UFP_WeaponBase> WeaponLogic;
};
