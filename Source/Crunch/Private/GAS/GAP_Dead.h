// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/CGameplayAbility.h"
#include "GAP_Dead.generated.h"

/**
 * 死亡的被动技能
 */
UCLASS()
class UGAP_Dead : public UCGameplayAbility
{
	GENERATED_BODY()

public:
	/**
	 * 死亡技能构造函数：配置技能触发条件
	 * 当角色收到"死亡"GameplayEvent时自动激活此技能
	 */
	UGAP_Dead();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;

private:
	// 奖励范围：以死亡角色为中心，检测奖励目标的球形半径
	UPROPERTY(EditAnywhere, Category = "Reward")
	float RewardRange = 1000.f;

	// 基础经验奖励：死亡角色提供的固定经验值
	UPROPERTY(EditAnywhere, Category = "Reward")
	float BaseExperienceReward = 200.f;

	// 基础金币奖励：死亡角色提供的固定金币值
	UPROPERTY(EditAnywhere, Category = "Reward")
	float BaseGoldReward = 200.f;

	// 经验值奖励系数：基于死亡角色经验值的奖励乘数
	UPROPERTY(EditAnywhere, Category = "Reward")
	float ExperienceRewardPerExperience = 0.1f;

	// 金币奖励系数：基于死亡角色经验值的金币乘数
	UPROPERTY(EditAnywhere, Category = "Reward")
	float GoldRewardPerExperience = 0.05f;

	// 击杀者奖励比例：击杀者获得的奖励占总奖励的比例
	UPROPERTY(EditAnywhere, Category = "Reward")
	float KillerRewardPortion = 0.5f;

	
	/**
	 * @brief 获取奖励目标
	 * @return 
	 */
	TArray<AActor*> GetRewardTargets() const;

	// 奖励效果
	UPROPERTY(EditDefaultsOnly, Category = "Reward")
	TSubclassOf<UGameplayEffect> RewardEffectClass;
};
