// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/MMC_BaseAttackDamage.h"
#include "GAS/CAttributeSet.h"

UMMC_BaseAttackDamage::UMMC_BaseAttackDamage()
{
	// 配置攻击力属性捕获（从施法者获取）
	// AttributeToCapture：要捕获的属性（攻击力）
	// AttributeSource：属性来源（Source=施法者，Target=目标）
	DamageCaptureDef.AttributeToCapture = UCAttributeSet::GetAttackDamageAttribute();
	DamageCaptureDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;

	// 配置护甲属性捕获（从目标获取）
	ArmorCaptureDef.AttributeToCapture = UCAttributeSet::GetArmorAttribute();
	ArmorCaptureDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;

	//注册要捕获的属性到列表（重要：让系统知道需要捕获哪些属性）
	RelevantAttributesToCapture.Add(DamageCaptureDef);
	RelevantAttributesToCapture.Add(ArmorCaptureDef);
}

float UMMC_BaseAttackDamage::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	// 创建评估参数：用于属性捕获的上下文信息
	FAggregatorEvaluateParameters EvalParams;

	// 设置GameplayTag条件：只考虑带有这些Tag的效果
	EvalParams.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();// 施法者的Tag
	EvalParams.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();// 目标的Tag

	// 捕获施法者的攻击力属性
	float AttackDamage = 0.f;// 输出参数：接收捕获到的攻击力值
	GetCapturedAttributeMagnitude(DamageCaptureDef,Spec,EvalParams,AttackDamage);

	// 捕获目标的护甲属性  
	float Armor = 0.f;// 输出参数：接收捕获到的护甲值
	GetCapturedAttributeMagnitude(ArmorCaptureDef,Spec,EvalParams,Armor);

	// 应用护甲减伤公式计算最终伤害
	// 护甲减伤公式：减伤比例 = 护甲 / (护甲 + 100)
	// 举例：50护甲 → 减伤33.3%；100护甲 → 减伤50%；200护甲 → 减伤66.7%
	float Damage = AttackDamage* (1-Armor / (Armor + 100));
	
	// 返回负值（因为伤害是减少生命值，所以用负数）
	// 正数 = 增益效果（如治疗、增加属性）
	// 负数 = 减益效果（如伤害、减少属性）
	return -Damage;
}
