// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GAS/CGameplayAbilityTypes.h"
#include "GameplayEffectTypes.h"
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
	UCAbilitySystemComponent();

	/**
	 * @brief 初始化角色基础属性：从DataTable中读取并设置角色的初始属性值
	 * 根据角色类匹配对应的基础属性配置
	 */
	void InitializeBaseAttributes();

	/**
	 * @brief 应用能力和初始化
	 */
	void ServerSideInit();

	/**
	 * @brief 将血量跟蓝量设置为最大血量跟蓝量,用于初始化
	 */
	void ApplyFullStatEffect();

	// 输入ID 与 技能对应表
	const TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>>& GetAbilities() const;

private:

	/**
	 * @brief 给予能力给玩家
	 */
	void GiveInitialAbilities();
	
	/**
	 * @brief 应用初始化Effect效果
	 */
	void ApplyInitialEffects();

	/**
	 * @brief 血量更新的时候调用
	 */
	void HealthUpdated(const FOnAttributeChangeData& ChangeData);

	void AuthApplyGameplayEffect(TSubclassOf<UGameplayEffect> GameplayEffect, int Level = 1);
	// 全部的状态效果
	UPROPERTY(EditAnywhere, Category="Gameplay Effects");
	TSubclassOf<UGameplayEffect> FullStatEffect;
	// 存放死亡的效果
	UPROPERTY(EditAnywhere, Category="Gameplay Effects");
	TSubclassOf<UGameplayEffect> DeathEffect;
	// 存放所有的Effect效果
	UPROPERTY(EditAnywhere, Category="Gameplay Effects");
	TArray<TSubclassOf<UGameplayEffect>> InitialEffect;

	// 存放所有的技能
	UPROPERTY(EditAnywhere, Category="Gameplay Ability");
	TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>> Abilities;
	// 存放所有基础的能力
	UPROPERTY(EditAnywhere, Category="Gameplay Ability");
	TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>> BasicAbilities;

	
	UPROPERTY(EditAnywhere, Category="Base Stats");
	UDataTable* BaseStatDataTable;
	
	
};
