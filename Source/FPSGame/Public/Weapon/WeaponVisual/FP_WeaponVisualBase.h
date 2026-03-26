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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> WeaponMesh;

	void Initialize(UFP_WeaponBase* NewWeaponLogic);
	
	UFUNCTION()
	void PlayFireMontage();
	
	UFUNCTION()
	void PlayReloadMontage();

	UFUNCTION(BlueprintPure, Category = "Weapon")
	FORCEINLINE UFP_WeaponBase* GetWeaponLogic() const { return WeaponLogic.Get(); }

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	TWeakObjectPtr<UFP_WeaponBase> WeaponLogic;
};
