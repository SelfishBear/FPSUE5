#include "Character/FP_BaseCharacter.h"

#include "Component/Character/FP_CharacterMovementComponent.h"
#include "Component/Character/FP_HealthComponent.h"
#include "Component/Character/FP_StaminaComponent.h"


AFP_BaseCharacter::AFP_BaseCharacter(const FObjectInitializer& ObjectInitializer) : Super(
	ObjectInitializer.SetDefaultSubobjectClass(CharacterMovementComponentName,
	                                           UFP_CharacterMovementComponent::StaticClass()))
{
	PrimaryActorTick.bCanEverTick = false;

	FP_MovementComponent = Cast<UFP_CharacterMovementComponent>(GetCharacterMovement());
	check(FP_MovementComponent);

	FP_HealthComponent = CreateDefaultSubobject<UFP_HealthComponent>(TEXT("FP_HealthComponent"));
	FP_StaminaComponent = CreateDefaultSubobject<UFP_StaminaComponent>(TEXT("FP_StaminaComponent"));
}

void AFP_BaseCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AFP_BaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFP_BaseCharacter::ReceiveDamage_Implementation(float DamageAmount)
{
	if (!IsValid(FP_HealthComponent)) return;
	FP_HealthComponent->TakeDamage(DamageAmount);
}

