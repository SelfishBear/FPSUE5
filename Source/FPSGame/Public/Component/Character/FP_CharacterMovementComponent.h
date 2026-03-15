// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "FP_CharacterMovementComponent.generated.h"


struct FInputActionValue;

UENUM(BlueprintType)
enum class EMovementDirection : uint8
{
	Forward  UMETA(DisplayName = "Forward"),
	Backward UMETA(DisplayName = "Backward"),
	Left     UMETA(DisplayName = "Left"),
	Right    UMETA(DisplayName = "Right"),
};

USTRUCT(BlueprintType)
struct FMovementSettings
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float WalkSpeed = 300.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float SprintSpeed = 600.0f;
};

USTRUCT(BlueprintType)
struct FMovementState
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "State")
	bool bSprinting = false;
};

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FPSGAME_API UFP_CharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UFP_CharacterMovementComponent();

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	FMovementSettings MovementSettings;

	UPROPERTY(BlueprintReadOnly, Category = "State")
	FMovementState State;

	UFUNCTION(BlueprintPure, Category = "Movement")
	EMovementDirection GetMovementDirection() const;
	
	/** Input Handlers */
	void Move(const FInputActionValue& Value);

	void StartSprinting();
	void StopSprinting();

protected:
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
