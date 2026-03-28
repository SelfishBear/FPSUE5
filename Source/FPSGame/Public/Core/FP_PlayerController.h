// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FP_PlayerController.generated.h"

class UInputMappingContext;

/**
 * 
 */
UCLASS()
class FPSGAME_API AFP_PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AFP_PlayerController();

	UFUNCTION(BlueprintCallable, Category = "Input")
	void SwitchContext(const UInputMappingContext* ContextToActivate);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> MainPlayerMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> ShopMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	int32 DefaultMappingPriority = 0;
};
