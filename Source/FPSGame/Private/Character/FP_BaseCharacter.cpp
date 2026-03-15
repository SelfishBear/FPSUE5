

#include "Character/FP_BaseCharacter.h"

#include "Component/Character/FP_CharacterMovementComponent.h"


AFP_BaseCharacter::AFP_BaseCharacter(const FObjectInitializer& ObjectInitializer) : Super(
	ObjectInitializer.SetDefaultSubobjectClass(CharacterMovementComponentName,
											   UFP_CharacterMovementComponent::StaticClass()))
{
	PrimaryActorTick.bCanEverTick = false;
	
}

void AFP_BaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	FP_MovementComponent = Cast<UFP_CharacterMovementComponent>(GetCharacterMovement());
	
}

void AFP_BaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
