// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Component/Character/FP_CustomCharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "FP_PlayerCharacter.generated.h"

class UFP_CustomCharacterMovementComponent;
class UCameraComponent;
class USpringArmComponent;
class UInputAction;
struct FInputActionValue;

UCLASS()
class FPSGAME_API AFP_PlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AFP_PlayerCharacter(const FObjectInitializer& ObjectInitializer);
	
	FORCEINLINE UFP_CustomCharacterMovementComponent* GetCustomMovementComponent() const { return CustomMovementComponent; }

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> FPSRoot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USpringArmComponent> CameraRoot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> CameraOffset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USpringArmComponent> MeshRoot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> MeshOffset;

	/** Input Actions */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> SprintAction;
	
	UPROPERTY()
	TObjectPtr<UFP_CustomCharacterMovementComponent> CustomMovementComponent;
	
	UFUNCTION(BlueprintCallable, Category = "Player|Movement")
	FORCEINLINE bool GetSprintIntention() const { return bWantsToSprint && !GetVelocity().IsNearlyZero(); }

protected:
	virtual void BeginPlay() override;
	
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
private:
	bool bWantsToSprint = false;
	
	/** Input Handlers */
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	void StartSprinting();
	void StopSprinting();
};
