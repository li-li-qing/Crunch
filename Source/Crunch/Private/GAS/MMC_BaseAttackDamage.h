// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_BaseAttackDamage.generated.h"

/**
 * 动态计算基础攻击伤害的MMC类
 * 计算公式：最终伤害 = 攻击力 × (1 - 护甲减伤比例)
 * 护甲减伤公式：护甲 / (护甲 + 100)
 */
UCLASS()
class UMMC_BaseAttackDamage : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	
	UMMC_BaseAttackDamage();

	/**
	 * @brief 实现伤害计算逻辑
	 * @param Spec 
	 * @return 
	 */
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

private:
	// 属性捕获定义：声明要捕获的属性
	FGameplayEffectAttributeCaptureDefinition DamageCaptureDef;// 捕获攻击力（来自施法者）
	FGameplayEffectAttributeCaptureDefinition ArmorCaptureDef;// 捕获护甲（来自目标）
};
