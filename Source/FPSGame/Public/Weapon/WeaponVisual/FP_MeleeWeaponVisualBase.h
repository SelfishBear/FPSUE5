// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FP_MeleeWeaponVisualBase.generated.h"

class UFP_MeleeWeaponBase;
class UFP_WeaponBase;

UCLASS()
class FPSGAME_API AFP_MeleeWeaponVisualBase : public AActor
{
	GENERATED_BODY()

public:
	AFP_MeleeWeaponVisualBase();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeleeWeaponMesh;

	void Initialize(UFP_MeleeWeaponBase* NewWeaponLogic);


	void PlayAttackMontage();

	UFUNCTION()
	void AttackAction();

	void ShowMelee();

	void HideMelee();

	UFUNCTION(BlueprintPure, Category = "Weapon")
	FORCEINLINE UFP_MeleeWeaponBase* GetMeleeLogic() const { return MeleeLogic.Get(); }

protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	TWeakObjectPtr<UFP_MeleeWeaponBase> MeleeLogic;
};
