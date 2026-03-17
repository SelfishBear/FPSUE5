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

EMovementDirection UFP_CharacterMovementComponent::GetMovementDirection() const
{
	if (!CharacterOwner) return EMovementDirection::Forward;

	const FVector CharacterVelocity = CharacterOwner->GetVelocity().GetSafeNormal2D();
	if (CharacterVelocity.IsNearlyZero()) return EMovementDirection::Forward;

	const FVector Forward = CharacterOwner->GetActorForwardVector();
	const FVector Right   = CharacterOwner->GetActorRightVector();

	const float ForwardDot = FVector::DotProduct(Forward, CharacterVelocity);
	const float RightDot   = FVector::DotProduct(Right,   CharacterVelocity);
	
	GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Green, FString::Printf(TEXT("ForwardDot: %f, RightDot: %f"), ForwardDot, RightDot));

	if (FMath::Abs(ForwardDot) >= FMath::Abs(RightDot))
	{
		return ForwardDot >= 0.f ? EMovementDirection::Forward : EMovementDirection::Backward;
	}
	return RightDot >= 0.f ? EMovementDirection::Right : EMovementDirection::Left;
}

