// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GAS/CGameplayAbilityTypes.h"
#include "CAbilitySystemComponent.generated.h"

/**
 *
 * 
 */
UCLASS()
class UCAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	/**
	 * @brief 应用初始化Effect效果
	 */
	void ApplyInitialEffects();

	/**
	 * @brief 给予能力给玩家
	 */
	void GiveInitialAbilities();

private:
	// 存放所有的Effect效果
	UPROPERTY(EditAnywhere, Category="Gameplay Effects");
	TArray<TSubclassOf<UGameplayEffect>> InitialEffect;

	// 存放所有的技能
	UPROPERTY(EditAnywhere, Category="Gameplay Effects");
	TMap<ECAbilityInputID,TSubclassOf<UGameplayAbility>> Abilities;
	// 存放所有基础的能力
	UPROPERTY(EditAnywhere, Category="Gameplay Effects");
	TMap<ECAbilityInputID,TSubclassOf<UGameplayAbility>> BasicAbilities;
};
