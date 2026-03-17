// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FP_BaseCharacter.h"
#include "Component/Character/FP_CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "FP_PlayerCharacter.generated.h"

class UFP_DynamicCameraComponent;
class UFP_CharacterMovementComponent;
class UCameraComponent;
class USpringArmComponent;
class UInputAction;
struct FInputActionValue;

UCLASS()
class FPSGAME_API AFP_PlayerCharacter : public AFP_BaseCharacter
{
	GENERATED_BODY()

public:
	AFP_PlayerCharacter(const FObjectInitializer& ObjectInitializer);
	
	/* Getters */
	UFUNCTION(BlueprintPure) 
	FORCEINLINE UCameraComponent* GetPlayerCamera() const { return Camera; }
	
	UFUNCTION(BlueprintPure)
	FORCEINLINE USceneComponent* GetCameraOffsetRoot() const { return CameraOffsetRoot; }

protected:
	virtual void BeginPlay() override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
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
	
	/** Input Actions */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> SprintAction;

private:
	/** Input Handlers */
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	void StartSprinting();
	void StopSprinting();
};
