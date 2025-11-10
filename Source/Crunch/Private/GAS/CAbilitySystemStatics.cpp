// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/CAbilitySystemStatics.h"
#include "Abilities/GameplayAbility.h"
FGameplayTag UCAbilitySystemStatics::GetBasicAttackAbilityTag()
{
	return FGameplayTag::RequestGameplayTag("Ability.BasicAttack");
}

FGameplayTag UCAbilitySystemStatics::GetBasicAttackInputPressedTag()
{
	return FGameplayTag::RequestGameplayTag("Ability.BasicAttack.Pressed");
}

FGameplayTag UCAbilitySystemStatics::GetDeadStatTag()
{
	return FGameplayTag::RequestGameplayTag("Stats.Dead");
}

FGameplayTag UCAbilitySystemStatics::GetStunStatTag()
{
	return FGameplayTag::RequestGameplayTag("Stats.Stun");
}

float UCAbilitySystemStatics::GetStaticCooldownDurationForAbility(const UGameplayAbility* Ability)
{
	// 安全检查：验证技能指针有效性
	if (!Ability) return 0.0f;

	//  获取技能关联的冷却效果（GameplayEffect）
	const UGameplayEffect* CooldownEffect = Ability->GetCooldownGameplayEffect();
    
	//  检查冷却效果是否存在
	if (!CooldownEffect) return 0.0f;

	// 初始化冷却时间变量
	float CooldownDuration = 0.f;

	//  从效果配置中提取静态冷却时长
	CooldownEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(1, CooldownDuration);
    
	//  返回提取的冷却时间
	return CooldownDuration;
}

float UCAbilitySystemStatics::GetStaticCostForAbility(const UGameplayAbility* Ability)
{
	// 安全检查：验证技能指针有效性
	if (!Ability) return 0.0f;

	// 获取技能关联的消耗效果（GameplayEffect）
	const UGameplayEffect* CostEffect = Ability->GetCostGameplayEffect();
	if (!CostEffect || CostEffect->Modifiers.Num() == 0) return 0.0f;

	// 初始化消耗变量
	float Cost = 0.f;
	// 从第一个效果修饰器中提取静态消耗值
	CostEffect->Modifiers[0].ModifierMagnitude.GetStaticMagnitudeIfPossible(1, Cost);

	// 返回提取的资源消耗
	return FMath::Abs(Cost);
	
}

FGameplayTag UCAbilitySystemStatics::GetAimStatTag()
{
	return FGameplayTag::RequestGameplayTag("Stats.Aim");
}

FGameplayTag UCAbilitySystemStatics::GetCameraShakeGameplayCueTag()
{
	return FGameplayTag::RequestGameplayTag("GameplayCue.CameraShake");
}

FGameplayTag UCAbilitySystemStatics::GetHealthFullStatTag()
{
	return FGameplayTag::RequestGameplayTag("Stats.Health.Full");
}

FGameplayTag UCAbilitySystemStatics::GetHealthEmptyStatTag()
{
	return FGameplayTag::RequestGameplayTag("Stats.Health.Empty");
}

FGameplayTag UCAbilitySystemStatics::GetManaFullStatTag()
{
	return FGameplayTag::RequestGameplayTag("Stats.Mana.Full");
}

FGameplayTag UCAbilitySystemStatics::GetManaEmptyStatTag()
{
	return FGameplayTag::RequestGameplayTag("Stats.Mana.Empty");
}
