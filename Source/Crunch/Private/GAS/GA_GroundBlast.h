// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/CGameplayAbility.h"
#include "GAS/CGameplayAbilityTypes.h"
#include "GA_GroundBlast.generated.h"


/**
 * 
 */
UCLASS()
class UGA_GroundBlast : public UCGameplayAbility
{
	GENERATED_BODY()
public:
	UGA_GroundBlast();
	/**
	 * 技能激活入口函数
	 * @param Handle 技能规格句柄，唯一标识技能实例
	 * @param ActorInfo 技能执行者的角色信息（包含Actor、ASC等）
	 * @param ActivationInfo 技能激活信息（包含网络权限、预测键等）
	 * @param TriggerEventData 触发技能的事件数据（如按键事件、伤害事件等）
	 */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
private:


	
	UPROPERTY(EditDefaultsOnly,Category="Damage")
	FGenericDamageEffectDef DamageEffectDef;
	
	UPROPERTY(EditDefaultsOnly,Category="Cue")
	FGameplayTag BlastGameplayCueTag;
	
	UPROPERTY(EditDefaultsOnly,Category="Targeting")
	float TargetAreaRadius = 300.f;

	UPROPERTY(EditDefaultsOnly,Category="Targeting")
	float TargetTraceRange = 2000.f;
	// 目标类
	UPROPERTY(EditDefaultsOnly,Category="Targeting")
	TSubclassOf<class ATargetActor_GroundPick> TargetActorClass;
	
	UPROPERTY(EditDefaultsOnly,Category="Animation")
	UAnimMontage* TargettingMontage;

	UPROPERTY(EditDefaultsOnly,Category="Animation")
	UAnimMontage* CastMontage;

	/**
	 * 目标确认回调函数（当玩家成功选择目标时调用）
	 * @param TargetDataHandle 包含目标数据（如命中位置、命中Actor等）的容器
	 */
	UFUNCTION()
	void TargetConfirmed(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	/**
	 * 目标取消回调函数（当玩家取消目标选择时调用）
	 * @param TargetDataHandle 目标数据容器（通常为空或无效）
	 */
	UFUNCTION()
	void TargetCancelled(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	
};
