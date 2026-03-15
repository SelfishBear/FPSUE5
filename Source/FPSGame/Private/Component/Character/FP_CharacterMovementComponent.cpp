// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/Character/FP_CharacterMovementComponent.h"

#include "InputActionValue.h"
#include "GameFramework/Character.h"

UFP_CharacterMovementComponent::UFP_CharacterMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UFP_CharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	
	MaxWalkSpeed = MovementSettings.WalkSpeed;
}

void UFP_CharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                         FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UFP_CharacterMovementComponent::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();

	if (!CharacterOwner || !CharacterOwner->GetController()) return;

	const FRotator Rotation = CharacterOwner->GetController()->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection   = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	CharacterOwner->AddMovementInput(ForwardDirection, MovementVector.Y);
	CharacterOwner->AddMovementInput(RightDirection,   MovementVector.X);
}

void UFP_CharacterMovementComponent::StartSprinting()
{
	if (State.bSprinting) return;

	State.bSprinting = true;
	
	MaxWalkSpeed = MovementSettings.SprintSpeed;
}

void UFP_CharacterMovementComponent::StopSprinting()
{
	if (!State.bSprinting) return;

	State.bSprinting = false;
	
	MaxWalkSpeed = MovementSettings.WalkSpeed;
}
