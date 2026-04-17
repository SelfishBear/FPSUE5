// Fill out your copyright notice in the Description page of Project Settings.


#include "Utils/FP_UtilsFunctionLibrary.h"

#include "DataAsset/Weapon/FP_WeaponDataAsset.h"

float UFP_UtilsFunctionLibrary::GetStatForLevel(UFP_WeaponDataAsset* WeaponData, FName StatName, int32 Level)
{
	if (!IsValid(WeaponData)) return 0.0f;
	
	static const FString ContextString = TEXT("WeaponLevelingContext");
	FRealCurve* Curve = WeaponData->WeaponLevelingTable->FindCurve(StatName, ContextString);
	if (!Curve) return 0.0f;	
	return Curve->Eval(static_cast<float>(Level));
}

void UFP_UtilsFunctionLibrary::SetGraphicsQualityLow()
{
	static const auto CVarGI = IConsoleManager::Get().FindConsoleVariable(TEXT("r.Lumen.DiffuseIndirect.Allow"));
	static const auto CVarLumenReflections = IConsoleManager::Get().FindConsoleVariable(TEXT("r.Lumen.Reflections.Allow"));
	static const auto CVarNanite = IConsoleManager::Get().FindConsoleVariable(TEXT("r.Nanite"));
	static const auto CVarShadows = IConsoleManager::Get().FindConsoleVariable(TEXT("r.ShadowQuality"));
	static const auto CVarAO = IConsoleManager::Get().FindConsoleVariable(TEXT("r.AmbientOcclusionLevels"));
	static const auto CVarBloom = IConsoleManager::Get().FindConsoleVariable(TEXT("r.BloomQuality"));
	static const auto CVarMotionBlur = IConsoleManager::Get().FindConsoleVariable(TEXT("r.MotionBlurQuality"));
	static const auto CVarAntiAliasing = IConsoleManager::Get().FindConsoleVariable(TEXT("r.AntiAliasingMethod"));
	static const auto CVarSSR = IConsoleManager::Get().FindConsoleVariable(TEXT("r.SSR.Quality"));
	static const auto CVarFog = IConsoleManager::Get().FindConsoleVariable(TEXT("r.VolumetricFog"));

	if (CVarGI)             CVarGI->Set(0);
	if (CVarLumenReflections) CVarLumenReflections->Set(0);
	if (CVarNanite)         CVarNanite->Set(0);
	if (CVarShadows)        CVarShadows->Set(0);
	if (CVarAO)             CVarAO->Set(0);
	if (CVarBloom)          CVarBloom->Set(0);
	if (CVarMotionBlur)     CVarMotionBlur->Set(0);
	if (CVarAntiAliasing)   CVarAntiAliasing->Set(0);
	if (CVarSSR)            CVarSSR->Set(0);
	if (CVarFog)            CVarFog->Set(0);

	// Scalability всё на низком
	Scalability::FQualityLevels Levels;
	Levels.SetFromSingleQualityLevel(0); // 0 = Low
	Scalability::SetQualityLevels(Levels);
}

void UFP_UtilsFunctionLibrary::SetGraphicsQualityHigh()
{
	static const auto CVarGI = IConsoleManager::Get().FindConsoleVariable(TEXT("r.Lumen.DiffuseIndirect.Allow"));
	static const auto CVarLumenReflections = IConsoleManager::Get().FindConsoleVariable(TEXT("r.Lumen.Reflections.Allow"));
	static const auto CVarNanite = IConsoleManager::Get().FindConsoleVariable(TEXT("r.Nanite"));
	static const auto CVarShadows = IConsoleManager::Get().FindConsoleVariable(TEXT("r.ShadowQuality"));
	static const auto CVarAO = IConsoleManager::Get().FindConsoleVariable(TEXT("r.AmbientOcclusionLevels"));
	static const auto CVarBloom = IConsoleManager::Get().FindConsoleVariable(TEXT("r.BloomQuality"));
	static const auto CVarMotionBlur = IConsoleManager::Get().FindConsoleVariable(TEXT("r.MotionBlurQuality"));
	static const auto CVarAntiAliasing = IConsoleManager::Get().FindConsoleVariable(TEXT("r.AntiAliasingMethod"));
	static const auto CVarSSR = IConsoleManager::Get().FindConsoleVariable(TEXT("r.SSR.Quality"));
	static const auto CVarFog = IConsoleManager::Get().FindConsoleVariable(TEXT("r.VolumetricFog"));

	if (CVarGI)             CVarGI->Set(1);
	if (CVarLumenReflections) CVarLumenReflections->Set(1);
	if (CVarNanite)         CVarNanite->Set(1);
	if (CVarShadows)        CVarShadows->Set(3);
	if (CVarAO)             CVarAO->Set(3);
	if (CVarBloom)          CVarBloom->Set(5);
	if (CVarMotionBlur)     CVarMotionBlur->Set(4);
	if (CVarAntiAliasing)   CVarAntiAliasing->Set(2); // TAA
	if (CVarSSR)            CVarSSR->Set(4);
	if (CVarFog)            CVarFog->Set(1);

	// Scalability обратно на Epic
	Scalability::FQualityLevels Levels;
	Levels.SetFromSingleQualityLevel(3); // 3 = Epic
	Scalability::SetQualityLevels(Levels);
}
