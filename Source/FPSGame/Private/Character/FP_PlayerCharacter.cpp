// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSGame/Public/Character/FP_PlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Component/Character/FP_CustomCharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AFP_PlayerCharacter::AFP_PlayerCharacter(const FObjectInitializer& ObjectInitializer) : Super(
	ObjectInitializer.SetDefaultSubobjectClass(CharacterMovementComponentName,
	                                           UFP_CustomCharacterMovementComponent::StaticClass()))
{
	PrimaryActorTick.bCanEverTick = false;

	FPSRoot = CreateDefaultSubobject<USceneComponent>(TEXT("FPSRoot"));
	FPSRoot->SetupAttachment(GetRootComponent());

	CameraRoot = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraRoot"));
	CameraRoot->SetupAttachment(FPSRoot);

	CameraOffset = CreateDefaultSubobject<USceneComponent>(TEXT("CameraOffset"));
	CameraOffset->SetupAttachment(CameraRoot);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraOffset);


	MeshRoot = CreateDefaultSubobject<USpringArmComponent>(TEXT("MeshRoot"));
	MeshRoot->SetupAttachment(FPSRoot);

	MeshOffset = CreateDefaultSubobject<USceneComponent>(TEXT("MeshOffset"));
	MeshOffset->SetupAttachment(MeshRoot);

	GetMesh()->SetupAttachment(MeshOffset);
}

void AFP_PlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	CustomMovementComponent = Cast<UFP_CustomCharacterMovementComponent>(GetCharacterMovement());
}

void AFP_PlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFP_PlayerCharacter::Move);

		EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFP_PlayerCharacter::Look);

		EnhancedInput->BindAction(SprintAction, ETriggerEvent::Started, this, &AFP_PlayerCharacter::StartSprinting);
		EnhancedInput->BindAction(SprintAction, ETriggerEvent::Completed, this, &AFP_PlayerCharacter::StopSprinting);

		EnhancedInput->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInput->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
	}
}

void AFP_PlayerCharacter::Move(const FInputActionValue& Value)
{
	if (CustomMovementComponent)
	{
		CustomMovementComponent->Move(Value);
	}
}

void AFP_PlayerCharacter::Look(const FInputActionValue& Value)
{
	if (CustomMovementComponent)
	{
		CustomMovementComponent->Look(Value);
	}
}

void AFP_PlayerCharacter::StartSprinting()
{
	bWantsToSprint = true;

	if (CustomMovementComponent)
	{
		CustomMovementComponent->StartSprinting();
	}
}

void AFP_PlayerCharacter::StopSprinting()
{
	bWantsToSprint = false;

	if (CustomMovementComponent)
	{
		CustomMovementComponent->StopSprinting();
	}
}
