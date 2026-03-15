// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSGame/Public/Character/FP_PlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Component/Camera/FP_DynamicCameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

AFP_PlayerCharacter::AFP_PlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	FPSRoot = CreateDefaultSubobject<USceneComponent>(TEXT("FPSRoot"));
	FPSRoot->SetupAttachment(GetRootComponent());

	CameraRoot = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraRoot"));
	CameraRoot->SetupAttachment(FPSRoot);

	CameraOffsetRoot = CreateDefaultSubobject<USceneComponent>(TEXT("CameraOffsetRoot"));
	CameraOffsetRoot->SetupAttachment(CameraRoot);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraOffsetRoot);

	MeshRoot = CreateDefaultSubobject<USpringArmComponent>(TEXT("MeshRoot"));
	MeshRoot->SetupAttachment(FPSRoot);

	MeshOffsetRoot = CreateDefaultSubobject<USceneComponent>(TEXT("MeshOffsetRoot"));
	MeshOffsetRoot->SetupAttachment(MeshRoot);

	GetMesh()->SetupAttachment(MeshOffsetRoot);
	
	DynamicCameraComponent = CreateDefaultSubobject<UFP_DynamicCameraComponent>(TEXT("DynamicCameraComponent"));
}

void AFP_PlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AFP_PlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	auto* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!IsValid(EnhancedInput)) return;

	EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFP_PlayerCharacter::Move);

	EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFP_PlayerCharacter::Look);

	EnhancedInput->BindAction(SprintAction, ETriggerEvent::Started, this, &AFP_PlayerCharacter::StartSprinting);
	EnhancedInput->BindAction(SprintAction, ETriggerEvent::Completed, this, &AFP_PlayerCharacter::StopSprinting);

	EnhancedInput->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
	EnhancedInput->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
}

void AFP_PlayerCharacter::Move(const FInputActionValue& Value)
{
	if (!IsValid(FP_MovementComponent)) return;

	FP_MovementComponent->Move(Value);
}

void AFP_PlayerCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (!GetController()) return;

	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(-LookAxisVector.Y);
}

void AFP_PlayerCharacter::StartSprinting()
{
	if (!IsValid(FP_MovementComponent)) return;

	if (GetVelocity().IsNearlyZero()) return;

	DynamicCameraComponent->IncreaseFov();
	FP_MovementComponent->StartSprinting();
}

void AFP_PlayerCharacter::StopSprinting()
{
	if (!IsValid(FP_MovementComponent)) return;

	DynamicCameraComponent->DecreaseFov();
	FP_MovementComponent->StopSprinting();
}
