// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_LevelBased.generated.h"

/**
 * 
 */
UCLASS()
class UMMC_LevelBased : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:

	UMMC_LevelBased();
	/**
	 * @brief 核心计算函数：实现基于等级的效果数值计算
	 * @param Spec GameplayEffect规格，包含所有上下文信息
	 * @return 计算出的效果数值（正数=增益，负数=减益）
	 */
	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

private:
	// 比率属性：用于计算效果强度的施法者属性
	UPROPERTY(EditDefaultsOnly)
	FGameplayAttribute RateAttribute;

	// 等级属性捕获定义：配置如何捕获目标的等级属性
	// 作用：告诉GAS系统需要捕获目标的等级值用于计算
	FGameplayEffectAttributeCaptureDefinition LevelCaptureDefination;
};
