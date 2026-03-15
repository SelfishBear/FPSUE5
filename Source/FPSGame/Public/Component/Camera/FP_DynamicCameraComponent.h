// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FP_DynamicCameraComponent.generated.h"

class UCameraComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FPSGAME_API UFP_DynamicCameraComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFP_DynamicCameraComponent();

protected:
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	bool TrySetFov(float DeltaTime);

public:
	UFUNCTION(BlueprintCallable, Category = "Camera|FOV")
	void IncreaseFov();

	UFUNCTION(BlueprintCallable, Category = "Camera|FOV")
	void DecreaseFov();

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Camera|FOV")
	TObjectPtr<UCameraComponent> CameraComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera|FOV")
	float BaseFov = 90.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera|FOV")
	float SprintFov = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera|FOV", meta = (ClampMin = "0.1"))
	float FovInterpSpeed = 5.0f;

private:
	float TargetFov = 90.0f;

	void GetPlayerCamera();
	void SetupFov();
};
