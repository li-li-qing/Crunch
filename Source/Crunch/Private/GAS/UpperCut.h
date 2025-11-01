// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/CGameplayAbility.h"
#include "GAS/CGameplayAbilityTypes.h"
#include "UpperCut.generated.h"

/**
 * @brief 上勾拳的能力
 */
UCLASS()
class UUpperCut : public UCGameplayAbility
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;

	UUpperCut();

private:
	UPROPERTY(EditDefaultsOnly, Category="Launch")
	TSubclassOf<UGameplayEffect> LaunchDamageEffect;
	
	UPROPERTY(EditDefaultsOnly, Category = "Launch")
	TMap<FName,FGenericDamageEffectDef> ComboDamageMap;
	// 击飞的速度
	UPROPERTY(EditDefaultsOnly, Category = "Launch")
	float UpperCutLaunchSpeed = 1000.f;
	// 击飞的速度
	UPROPERTY(EditDefaultsOnly, Category = "Launch")
	float UpperComboHoldSpeed = 100.f;
	// 伤害检测的范围大小
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetSweepSphereRadius = 80.f;
	// 上勾拳的蒙太奇动画
	UPROPERTY(EditAnywhere, Category = "Animation")
	TObjectPtr<UAnimMontage> UpperCutMontage;

	/**
	 * @brief 获取当前连招动作对应的伤害效果配置
	 * @return 
	 */
	const FGenericDamageEffectDef* GetDamageEffectDerForCurrentCombo() const;
	/**
	 * @brief 获得上勾拳的启动标签
	 * @return 
	 */
	static FGameplayTag GetUpperCutLaunchTag();

	/**
	 * @brief 接受到开始上勾拳事件
	 */
	UFUNCTION()
	void StartLaunching(FGameplayEventData EventData);

	

	/**
	 * @brief 处理连招变化事件，更新下一个连招动作
	 * @param EventData 包含事件标签和相关信息的数据结构
	 */
	UFUNCTION()
	void HandleComboChangeEvent(FGameplayEventData EventData);
	
	UFUNCTION()
	void HandleComboCommitEvent(FGameplayEventData EventData);
	
	UFUNCTION()
	void HandleComboDamageEvent(FGameplayEventData EventData);

	// 下一个组合的名字
	FName NextComboName;
};




