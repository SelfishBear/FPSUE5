
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FP_BaseCharacter.generated.h"

class UFP_CharacterMovementComponent;

UCLASS()
class FPSGAME_API AFP_BaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AFP_BaseCharacter(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UFP_CharacterMovementComponent> FP_MovementComponent;

public:
	/* Getters */
	UFUNCTION(BlueprintPure) 
	FORCEINLINE UFP_CharacterMovementComponent* GetFP_MovementComponent() const { return FP_MovementComponent; }
};
