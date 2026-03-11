// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSGame/Public/Character/FP_PlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AFP_PlayerCharacter::AFP_PlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

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
}

void AFP_PlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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
	const FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AFP_PlayerCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(-LookAxisVector.Y);
	}
}

void AFP_PlayerCharacter::StartSprinting()
{
	UE_LOG(LogTemp, Warning, TEXT("StartSprinting"));
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void AFP_PlayerCharacter::StopSprinting()
{
	UE_LOG(LogTemp, Warning, TEXT("StopSprinting"));
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}
