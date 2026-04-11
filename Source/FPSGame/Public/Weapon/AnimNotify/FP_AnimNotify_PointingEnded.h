// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "FP_AnimNotify_PointingEnded.generated.h"

/**
 * 
 */
UCLASS()
class FPSGAME_API UFP_AnimNotify_PointingEnded : public UAnimNotify
{
	GENERATED_BODY()
public:
	virtual FString GetNotifyName_Implementation() const override { return TEXT("PointingEnded"); }

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
						const FAnimNotifyEventReference& EventReference) override;
};
