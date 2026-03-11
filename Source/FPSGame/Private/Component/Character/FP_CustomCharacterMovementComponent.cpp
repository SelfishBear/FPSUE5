// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/Character/FP_CustomCharacterMovementComponent.h"

#include "InputActionValue.h"
#include "GameFramework/Character.h"

UFP_CustomCharacterMovementComponent::UFP_CustomCharacterMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UFP_CustomCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	
	MaxWalkSpeed = MovementSettings.WalkSpeed;
}

void UFP_CustomCharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                         FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UFP_CustomCharacterMovementComponent::Move(const FInputActionValue& Value)
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

void UFP_CustomCharacterMovementComponent::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (!CharacterOwner || !CharacterOwner->GetController()) return;

	CharacterOwner->AddControllerYawInput(LookAxisVector.X);
	CharacterOwner->AddControllerPitchInput(-LookAxisVector.Y);
}

void UFP_CustomCharacterMovementComponent::StartSprinting()
{
	MaxWalkSpeed = MovementSettings.SprintSpeed;
}

void UFP_CustomCharacterMovementComponent::StopSprinting()
{
	MaxWalkSpeed = MovementSettings.WalkSpeed;
}
