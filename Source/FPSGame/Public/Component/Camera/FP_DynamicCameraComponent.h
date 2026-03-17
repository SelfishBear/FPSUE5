// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FP_DynamicCameraComponent.generated.h"

class UFP_CharacterMovementComponent;
class UCameraComponent;

USTRUCT(BlueprintType)
struct FCameraFovSettings
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera|FOV")
	float BaseFov = 90.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera|FOV")
	float SprintFov = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera|FOV", meta = (ClampMin = "0.1"))
	float FovInterpSpeed = 5.0f;
};

USTRUCT(BlueprintType)
struct FCameraTiltSettings
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera|Tilt")
	float MaxTiltAngle = 10.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera|Tilt", meta = (ClampMin = "0.1"))
	float TiltInterpSpeed = 5.0f;
};

USTRUCT(BlueprintType)
struct FBobbingParams
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera|Bobbing")
	float Frequency = 10.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera|Bobbing")
	float VerticalAmplitude = 2.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera|Bobbing")
	float HorizontalAmplitude = 1.0f;
};

USTRUCT(BlueprintType)
struct FBobbingSettings
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera|Bobbing")
	FBobbingParams Walk;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera|Bobbing")
	FBobbingParams Sprint = {14.0f, 3.5f, 2.0f};

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera|Bobbing", meta = (ClampMin = "0.1"))
	float ReturnInterpSpeed = 10.0f;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FPSGAME_API UFP_DynamicCameraComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFP_DynamicCameraComponent();

protected:
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	bool TrySetFov(float DeltaTime);

public:
	UFUNCTION(BlueprintCallable, Category = "Camera|FOV")
	void IncreaseFov();

	UFUNCTION(BlueprintCallable, Category = "Camera|FOV")
	void DecreaseFov();

	UFUNCTION(BlueprintCallable, Category = "Camera|FOV")
	void HandleCameraTilt(float DeltaTime);
	
	UFUNCTION(BlueprintCallable, Category = "Camera|FOV")
	void HandleCameraBobbing(float DeltaTime);

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Camera|FOV")
	TObjectPtr<UCameraComponent> CameraComponent;

	UPROPERTY(BlueprintReadOnly, Category = "Camera|Movement")
	TObjectPtr<UFP_CharacterMovementComponent> FP_MovementComponent;

	UPROPERTY(BlueprintReadOnly, Category = "Camera|Movement")
	TObjectPtr<USceneComponent> CameraOffsetRoot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera|FOV")
	FCameraFovSettings FovSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera|Tilt")
	FCameraTiltSettings TiltSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera|Tilt")
	FBobbingSettings BobbingSettings;

private:
	float TargetFov = 90.0f;
	float TargetTilt = 0.0f;
	
	float BobTimer = 0.0f;
	FVector CurrentBobOffset = FVector::ZeroVector;
	FVector BaseOffsetLocation = FVector::ZeroVector;
	
	FBobbingParams ActiveBobbingParams;

	void GetPlayerCamera();
	void GetPlayerCharacterMovementComponent();
	void GetCameraOffsetRoot();

	void SetupFov();
};
