// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/CGameplayAbility.h"
#include "GA_Combo.generated.h"

/**
 * 
 */
UCLASS()
class UGA_Combo : public UCGameplayAbility
{
	GENERATED_BODY()
public:
	UGA_Combo();
	/**
	 * @brief 连招技能（Combo Ability）的激活逻辑
	 * @param Handle 技能实例的唯一标识
	 * @param ActorInfo 施法者信息（如Actor、ASC等）
	 * @param ActivationInfo 激活信息（输入来源、触发方式等）
	 * @param TriggerEventData 触发技能的事件数据（可选）
	 */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	/**
	 * @brief 获取连招切换事件的GameplayTag
	 * @return 标签"Ability.Combo.Change"（需在DefaultGameplayTags.ini中定义）
	 */
	static FGameplayTag GetCombaChangedEventTag();
	/**
	 * @brief 获取连招结束事件的GameplayTag
	 * @return 标签"Ability.Combo.Change.End"
	 */
	static FGameplayTag GetCombaChangedEventEndTag();
	/**
	 * @brief 获得攻击目标点的标签 (Ability.Combo.Damage)
	 * @return 
	 */
	static FGameplayTag GetComboTargetEventTag();
private:
	// 战斗的蒙太奇
	UPROPERTY(EditDefaultsOnly,Category="Animation")
	UAnimMontage* ComboMontage;
	
	/**
	 * @brief 处理接收到的GameplayEvent
	 * @param Data 事件数据（包含标签、Instigator、Target等）
	 */
	UFUNCTION()
	void ComboChangedEventReceived(FGameplayEventData Data);
	/**
	 * @brief 通过标签层级传递连招段信息
	 */
	void SetupWaitComboInputPress();

	/**
	 * @brief 每次输入后重新绑定监听，实现连续输入检测
	 * @param TimeWaited 
	 */
	UFUNCTION()
	void HandleInputPress(float TimeWaited);

	/**
	 * @brief 对命中的目标应用伤害效果
	 * @param Data 包含目标数据的GameplayEvent（如攻击命中事件）
	 */
	UFUNCTION()
	void DoDamage(FGameplayEventData Data);
	/**
	 * @brief 尝试切换下一个蒙太奇
	 */
	void TryCommitCombo();

	// 默认伤害效果 当在 DamageEffectMap中找不到匹配的连招段时，使用此默认效果。
	UPROPERTY(EditDefaultsOnly,Category="Gameplay Effect")
	TSubclassOf<UGameplayEffect> DefaultDamageEffect;
	// 连招段伤害效果映射表
	UPROPERTY(EditDefaultsOnly,Category="Gameplay Effect")
	TMap<FName,TSubclassOf<UGameplayEffect>> DamageEffectMap;
	// 击打的范围大小
	UPROPERTY(EditDefaultsOnly,Category="Targetting")
	float TargetSweepSphereRadius = 30.f;
	/**
	 * @brief 根据当前连招段获取对应的伤害效果（GameplayEffect）
	 * @return 返回当前连招段对应的GameplayEffect类，如果未找到返回nullptr
	 */
	TSubclassOf<UGameplayEffect> GetDamageEffectForCurrentCombo()const;
	
	// 下一个战斗插槽的名字
	FName NextComboName;
};
