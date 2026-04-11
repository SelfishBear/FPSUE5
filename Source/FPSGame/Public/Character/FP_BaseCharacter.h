
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/FP_Damageable.h"
#include "FP_BaseCharacter.generated.h"

class UFP_StaminaComponent;
class UFP_HealthComponent;
class UFP_CharacterMovementComponent;

UCLASS()
class FPSGAME_API AFP_BaseCharacter : public ACharacter, public IFP_Damageable
{
	GENERATED_BODY()

public:
	AFP_BaseCharacter(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UFP_CharacterMovementComponent> FP_MovementComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UFP_HealthComponent> FP_HealthComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UFP_StaminaComponent> FP_StaminaComponent;

public:
	/* IFP_Damageable */
	virtual void ReceiveDamage_Implementation(float DamageAmount) override;

	/* Getters */
	UFUNCTION(BlueprintPure) 
	FORCEINLINE UFP_CharacterMovementComponent* GetFP_MovementComponent() const { return FP_MovementComponent; }
	
	UFUNCTION(BlueprintPure)
	FORCEINLINE UFP_HealthComponent* GetFP_HealthComponent() const { return FP_HealthComponent; }
	
	UFUNCTION(BlueprintPure)
	FORCEINLINE UFP_StaminaComponent* GetFP_StaminaComponent() const { return FP_StaminaComponent; }
};
