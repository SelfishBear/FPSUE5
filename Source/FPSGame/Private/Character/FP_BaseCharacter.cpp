

#include "Character/FP_BaseCharacter.h"

#include "Component/Character/FP_CharacterMovementComponent.h"


AFP_BaseCharacter::AFP_BaseCharacter(const FObjectInitializer& ObjectInitializer) : Super(
	ObjectInitializer.SetDefaultSubobjectClass(CharacterMovementComponentName,
											   UFP_CharacterMovementComponent::StaticClass()))
{
	PrimaryActorTick.bCanEverTick = false;

	FP_MovementComponent = Cast<UFP_CharacterMovementComponent>(GetCharacterMovement());
	check(FP_MovementComponent);
}

void AFP_BaseCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AFP_BaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
