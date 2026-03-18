// Fill out your copyright notice in the Description page of Project Settings.

#include "Component/Camera/FP_DynamicCameraComponent.h"
#include "Camera/CameraComponent.h"
#include "Character/FP_PlayerCharacter.h"
#include "Kismet/KismetMathLibrary.h"

UFP_DynamicCameraComponent::UFP_DynamicCameraComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UFP_DynamicCameraComponent::Init()
{
	GetPlayerCharacterMovementComponent();
	GetPlayerCamera();
	GetCameraOffsetRoot();
	SetupFov();
}

void UFP_DynamicCameraComponent::BeginPlay()
{
	Super::BeginPlay();

	Init();
}

void UFP_DynamicCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                               FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!IsValid(CameraComponent)) return;

	TrySetFov(DeltaTime);
	HandleCameraTilt(DeltaTime);
	HandleCameraBobbing(DeltaTime);
}

void UFP_DynamicCameraComponent::HandleCameraTilt(float DeltaTime)
{
	if (!IsValid(CameraComponent)) return;
	if (!IsValid(FP_MovementComponent)) return;

	const EMovementDirection Direction = FP_MovementComponent->GetMovementDirection();
	switch (Direction)
	{
	case EMovementDirection::Left: TargetTilt = -TiltSettings.MaxTiltAngle;
		break;
	case EMovementDirection::Right: TargetTilt = TiltSettings.MaxTiltAngle;
		break;
	default: TargetTilt = 0.0f;
		break;
	}

	const FRotator CurrentRotation = CameraComponent->GetRelativeRotation();
	const FRotator TargetRotation = FRotator(CurrentRotation.Pitch, CurrentRotation.Yaw, TargetTilt);
	const FRotator NewRotation = UKismetMathLibrary::RInterpTo(CurrentRotation, TargetRotation, DeltaTime,
	                                                           TiltSettings.TiltInterpSpeed);
	CameraComponent->SetRelativeRotation(NewRotation);
}

void UFP_DynamicCameraComponent::HandleCameraBobbing(float DeltaTime)
{
	if (!IsValid(CameraOffsetRoot)) return;
	if (!IsValid(FP_MovementComponent)) return;

	FVector TargetOffset = FVector::ZeroVector;
	const float Speed = FP_MovementComponent->Velocity.Size2D();

	if (Speed > 10.0f)
	{
		ActiveBobbingParams = FP_MovementComponent->State.bSprinting
			                      ? BobbingSettings.Sprint
			                      : BobbingSettings.Walk;

		BobTimer += DeltaTime * ActiveBobbingParams.Frequency;

		TargetOffset.Z = FMath::Sin(BobTimer) * ActiveBobbingParams.VerticalAmplitude;
		TargetOffset.Y = FMath::Cos(BobTimer * 0.5f) * ActiveBobbingParams.HorizontalAmplitude;
	}

	CurrentBobOffset = FMath::VInterpTo(CurrentBobOffset, TargetOffset, DeltaTime, BobbingSettings.ReturnInterpSpeed);

	CameraOffsetRoot->SetRelativeLocation(BaseOffsetLocation + CurrentBobOffset);
}

bool UFP_DynamicCameraComponent::TrySetFov(float DeltaTime)
{
	const float CurrentFov = CameraComponent->FieldOfView;
	if (FMath::IsNearlyEqual(CurrentFov, TargetFov, 0.01f)) return false;

	const float NewFov = UKismetMathLibrary::FInterpTo(CurrentFov, TargetFov, DeltaTime, FovSettings.FovInterpSpeed);
	CameraComponent->SetFieldOfView(NewFov);
	return true;
}

void UFP_DynamicCameraComponent::IncreaseFov()
{
	TargetFov = FovSettings.SprintFov;
}

void UFP_DynamicCameraComponent::DecreaseFov()
{
	TargetFov = FovSettings.BaseFov;
}

void UFP_DynamicCameraComponent::SetupFov()
{
	TargetFov = FovSettings.BaseFov;

	if (!IsValid(CameraComponent)) return;
	CameraComponent->SetFieldOfView(TargetFov);
}

void UFP_DynamicCameraComponent::GetPlayerCharacterMovementComponent()
{
	if (AFP_PlayerCharacter* CharacterOwner = Cast<AFP_PlayerCharacter>(GetOwner()))
	{
		FP_MovementComponent = CharacterOwner->GetFP_MovementComponent();
	}
}

void UFP_DynamicCameraComponent::GetCameraOffsetRoot()
{
	if (AFP_PlayerCharacter* CharacterOwner = Cast<AFP_PlayerCharacter>(GetOwner()))
	{
		CameraOffsetRoot = CharacterOwner->GetCameraOffsetRoot();
		if (!IsValid(CameraOffsetRoot)) return;
		BaseOffsetLocation = CameraOffsetRoot->GetRelativeLocation();
	}
}

void UFP_DynamicCameraComponent::GetPlayerCamera()
{
	if (AFP_PlayerCharacter* CharacterOwner = Cast<AFP_PlayerCharacter>(GetOwner()))
	{
		CameraComponent = CharacterOwner->GetPlayerCamera();
	}
}
