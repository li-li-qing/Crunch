// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/CAbilitySystemStatics.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemInterface.h"

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

FGameplayTag UCAbilitySystemStatics::GetHeroRoleTag()
{
	return FGameplayTag::RequestGameplayTag("Role.Hero");
}

FGameplayTag UCAbilitySystemStatics::GetExperienceAttributeTag()
{
	return FGameplayTag::RequestGameplayTag("Attribute.Experience");
}

FGameplayTag UCAbilitySystemStatics::GetGoldAttributeTag()
{
	return FGameplayTag::RequestGameplayTag("Attribute.Gold");
}

bool UCAbilitySystemStatics::IsHero(const AActor* ActorToCheck)
{
	// 将Actor转换为AbilitySystemInterface接口
	// IAbilitySystemInterface是GAS系统的核心接口，用于获取AbilitySystemComponent
	const IAbilitySystemInterface* ActorISA = Cast<IAbilitySystemInterface>(ActorToCheck);
	// 检查Actor是否实现了IAbilitySystemInterface接口
	if (ActorISA)
	{
		// 通过接口获取Actor的AbilitySystemComponent（GAS组件）
		// AbilitySystemComponent是GAS系统的核心组件，负责管理技能、属性和效果
		UAbilitySystemComponent* ActorASC = ActorISA->GetAbilitySystemComponent();
		      
		// 检查是否成功获取到AbilitySystemComponent
		if (ActorASC)
		{
			// 检查Actor是否拥有"Role.Hero"的GameplayTag
			// GetHeroRoleTag()返回标识英雄角色的标签（如"Role.Hero"）
			// HasMatchingGameplayTag()检查组件是否包含指定的标签
			return ActorASC->HasMatchingGameplayTag(GetHeroRoleTag());
		}
	}
	// 如果Actor没有实现接口或没有GAS组件，返回false（不是英雄）
	return false;
}
