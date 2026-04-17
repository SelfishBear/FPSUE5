// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FP_BaseCharacter.h"
#include "Component/Character/FP_CharacterMovementComponent.h"
#include "FP_PlayerCharacter.generated.h"

class UFP_AudioComponent;
class UFP_PointKillAbility;
class UFP_WalletComponent;
class UFP_DynamicCameraComponent;
class UFP_CharacterMovementComponent;
class UFP_EquipmentManager;
class UCameraComponent;
class USpringArmComponent;
class UInputAction;
struct FInputActionValue;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWantToStart);

UCLASS()
class FPSGAME_API AFP_PlayerCharacter : public AFP_BaseCharacter
{
	GENERATED_BODY()

public:
	AFP_PlayerCharacter(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "Player|SpecialAbility")
	void PerformPointKill();

	/* Getters */
	UFUNCTION(BlueprintPure)
	FORCEINLINE UCameraComponent* GetPlayerCamera() const { return Camera; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE USceneComponent* GetCameraOffsetRoot() const { return CameraOffsetRoot; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE UFP_EquipmentManager* GetEquipmentManager() const { return EquipmentManager; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE UFP_WalletComponent* GetWalletComponent() const { return WalletComponent; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE UFP_AudioComponent* GetFP_AudioComponent() const { return FP_AudioComponent; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE UFP_PointKillAbility* GetPointKillAbilityComponent() const { return PointKillAbilityComponent; }
	
	UPROPERTY(BlueprintAssignable, Category="GameFlow")
	FOnWantToStart OnWantToStart;

protected:
	virtual void BeginPlay() override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void SetupStarterSpeed();
	
	UFUNCTION()
	void HandleDeath();

	/** Components */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> FPSRoot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USpringArmComponent> CameraRoot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> CameraOffsetRoot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USpringArmComponent> MeshRoot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> MeshOffsetRoot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UFP_DynamicCameraComponent> DynamicCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UFP_EquipmentManager> EquipmentManager;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UFP_WalletComponent> WalletComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UFP_AudioComponent> FP_AudioComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UFP_PointKillAbility> PointKillAbilityComponent;

	/** Input Actions */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> SprintAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> FireAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> DashAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> ReloadAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> SwitchWeaponAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> ScrollWeaponAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> StabAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> PointKillAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> WantToStartAction;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> OpenSettingsAction;

private:
	/** Input Handlers */
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	void StartSprinting();
	void StopSprinting();

	void StartFire();
	void StopFire();
	void HandleReload();
	void HandleSwitchWeaponAction(const FInputActionValue& Value);
	void HandleScrollWeapon(const FInputActionValue& Value);

	void Dash();

	void Stab();

	void WantToStart();

	UFUNCTION()
	void CheckStamina(float NewStamina);
	
	void OpenSettings();
	
	bool bSettingsOpen = false;
	
	UPROPERTY()
	UUserWidget* PauseMenuWidget;
};
