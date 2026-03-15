// Fill out your copyright notice in the Description page of Project Settings.

#include "Component/Camera/FP_DynamicCameraComponent.h"
#include "Camera/CameraComponent.h"
#include "Character/FP_PlayerCharacter.h"
#include "Kismet/KismetMathLibrary.h"

UFP_DynamicCameraComponent::UFP_DynamicCameraComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UFP_DynamicCameraComponent::BeginPlay()
{
	Super::BeginPlay();

	GetPlayerCamera();
	SetupFov();
}

void UFP_DynamicCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                               FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!IsValid(CameraComponent)) return;

	TrySetFov(DeltaTime);
}

bool UFP_DynamicCameraComponent::TrySetFov(float DeltaTime)
{
	const float CurrentFov = CameraComponent->FieldOfView;
	if (FMath::IsNearlyEqual(CurrentFov, TargetFov, 0.01f)) return false;

	const float NewFov = UKismetMathLibrary::FInterpTo(CurrentFov, TargetFov, DeltaTime, FovInterpSpeed);
	CameraComponent->SetFieldOfView(NewFov);
	return true;
}

void UFP_DynamicCameraComponent::IncreaseFov()
{
	TargetFov = SprintFov;
}

void UFP_DynamicCameraComponent::DecreaseFov()
{
	TargetFov = BaseFov;
}

void UFP_DynamicCameraComponent::GetPlayerCamera()
{
	if (AFP_PlayerCharacter* CharacterOwner = Cast<AFP_PlayerCharacter>(GetOwner()))
	{
		CameraComponent = CharacterOwner->GetPlayerCamera();
	}
}

void UFP_DynamicCameraComponent::SetupFov()
{
	TargetFov = BaseFov;

	if (!IsValid(CameraComponent)) return;

	CameraComponent->SetFieldOfView(BaseFov);
}
