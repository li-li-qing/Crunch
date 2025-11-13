// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/MMC_LevelBased.h"

#include "CHeroAttributeSet.h"

UMMC_LevelBased::UMMC_LevelBased()
{
	// 配置等级属性捕获定义
	// AttributeSource = Target：从目标（效果接收者）捕获属性值
	// 作用：计算时将使用目标的等级属性值
	LevelCaptureDefination.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	// AttributeToCapture = LevelAttribute：指定要捕获的属性是等级属性
	// 作用：告诉系统需要捕获目标的等级属性值用于计算
	LevelCaptureDefination.AttributeToCapture = UCHeroAttributeSet::GetLevelAttribute();

	// 将属性捕获定义添加到相关属性列表中
	// 作用：注册这个捕获定义，让系统知道在计算时需要获取目标的等级值
	RelevantAttributesToCapture.Add(LevelCaptureDefination);
}

float UMMC_LevelBased::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	// 获取施法者的AbilitySystemComponent（ASC）
	// Spec.GetContext().GetInstigatorAbilitySystemComponent() 返回应用此效果的施法者的GAS组件
	UAbilitySystemComponent* ASC = Spec.GetContext().GetInstigatorAbilitySystemComponent();
	if (!ASC) return 0.0f;
	// 初始化等级变量，用于存储捕获的目标等级
	float Level = 0;
	// 创建评估参数结构，用于属性捕获
	FAggregatorEvaluateParameters EvalParams;
	// 设置源标签（施法者的GameplayTag）
	// Spec.CapturedSourceTags.GetAggregatedTags() 获取施法者所有的GameplayTag
	EvalParams.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	// 设置目标标签（目标的GameplayTag）
	// Spec.CapturedTargetTags.GetAggregatedTags() 获取目标所有的GameplayTag
	EvalParams.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	// 捕获目标等级值
	// GetCapturedAttributeMagnitude() 从目标捕获配置的等级属性值
	// LevelCaptureDefination 是在构造函数中配置的等级属性捕获定义
	// 结果存储在 Level 变量中
	GetCapturedAttributeMagnitude(LevelCaptureDefination,Spec,EvalParams,Level);

	// 获取施法者的比率属性值
	bool bFound;
	// ASC->GetGameplayAttributeValue() 从施法者获取指定的属性当前值
	// RateAttribute 
	// bFound 输出参数，指示是否成功找到属性
	float RateAttributeVal = ASC->GetGameplayAttributeValue(RateAttribute, bFound);
    
	// 如果找不到比率属性，返回0
	if (!bFound) 
		return 0.0f;

	// 计算公式：效果值 = (目标等级 - 1) × 施法者比率属性
	// (Level - 1)：从等级1开始计算（等级1的效果为0）
	// RateAttributeVal：施法者的属性值（决定效果强度）
	return (Level - 1) * RateAttributeVal;
}
