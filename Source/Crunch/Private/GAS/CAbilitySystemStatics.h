// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CAbilitySystemStatics.generated.h"
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

	
};
