// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CAbilitySystemStatics.generated.h"

class UGameplayAbility;
struct FGameplayAbilitySpec;
class UAbilitySystemComponent;
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

	/**
	 * @brief 获得瞄准标签
	 * @return 
	 */
	static FGameplayTag GetAimStatTag();

	static FGameplayTag GetCameraShakeGameplayCueTag();

	static FGameplayTag GetHealthFullStatTag();
	static FGameplayTag GetHealthEmptyStatTag();
	
	static FGameplayTag GetManaFullStatTag();
	static FGameplayTag GetManaEmptyStatTag();
	
	static FGameplayTag GetHeroRoleTag();
	
	static FGameplayTag GetExperienceAttributeTag();

	static FGameplayTag GetGoldAttributeTag();


	/**
	 * @brief 检查一个Actor是否是英雄角色
	 * @param ActorToCheck 
	 * @return 
	 */
	static bool IsHero(const AActor* ActorToCheck);

	static bool IsAbilityAtMaxLevel(const FGameplayAbilitySpec& Spec);

	/**
	 * @brief // 检查技能消耗是否足够的静态辅助函数
	 * @param AbilitySpec 
	 * @param AbilitySystemComponent 
	 * @return 
	 */
	static bool CheckAbilityCost(const FGameplayAbilitySpec& AbilitySpec,const UAbilitySystemComponent& AbilitySystemComponent);

	static float GetManaCostFor(const UGameplayAbility* AbilityCDO,const UAbilitySystemComponent& ASC,int AbilityLevel);
	
	static float GetCooldownDurationFor(const UGameplayAbility* AbilityCDO,const UAbilitySystemComponent& ASC,int AbilityLevel);
	
	static float GetCooldownRemainingFor(const UGameplayAbility* AbilityCDO,const UAbilitySystemComponent& ASC,int AbilityLevel);
};
