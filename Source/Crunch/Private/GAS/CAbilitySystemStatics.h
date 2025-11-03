// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CAbilitySystemStatics.generated.h"
class UGameplayAbility;
/**
 * 
 */
UCLASS()
class UCAbilitySystemStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * @brief 获取基础攻击技能的游戏标签（GameplayTag）
	 * @return 
	 */
	static FGameplayTag GetBasicAttackAbilityTag();

	static FGameplayTag GetBasicAttackInputPressedTag();
	/**
	 * @brief 获得死亡状态标签
	 * @return 
	 */
	static FGameplayTag GetDeadStatTag();

	/**
	 * @brief 获得眩晕标签
	 * @return 
	 */
	static FGameplayTag GetStunStatTag();

	/**
	 * 获取技能的静态冷却时间（从GameplayEffect配置中提取）
	 * @param Ability 要查询的技能对象
	 * @return 冷却时间（秒），无效时返回0.0f
	 */
	static float GetStaticCooldownDurationForAbility(const UGameplayAbility* Ability);

	/**
	 * 获取技能的静态资源消耗（从GameplayEffect配置中提取）
	 * @param Ability 要查询的技能对象
	 * @return 资源消耗量，无效时返回0.0f
	 */
	static float GetStaticCostForAbility(const UGameplayAbility* Ability);

	
};
