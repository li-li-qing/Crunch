// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "Abilities/GameplayAbility.h"
#include "CGameplayAbility.generated.h"
class UAnimInstance;
/**
 * 
 */
UCLASS()
class UCGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UCGameplayAbility();
	/**
	 * @brief 检查技能是否可以激活的覆写函数
	 * 
	 * 这是Gameplay Ability System (GAS) 中技能激活前的核心检查函数。
	 * 在技能尝试激活前调用，用于验证所有激活条件是否满足。
	 * 
	 * @param Handle 技能的唯一标识句柄，用于在AbilitySystemComponent中查找技能实例
	 * @param ActorInfo 包含技能拥有者信息的结构体，提供Actor、ASC等关键信息
	 * @param SourceTags 源标签容器（可选），包含技能施法者的GameplayTag
	 * @param TargetTags 目标标签容器（可选），包含技能目标的GameplayTag  
	 * @param OptionalRelevantTags 输出参数（可选），用于返回技能无法激活的相关原因标签
	 * @return bool true=技能可以激活，false=技能无法激活
	 * 
	 * @note 此函数在以下情况被调用：
	 * - 玩家按下技能按键时
	 * - AI尝试使用技能时  
	 * - 技能被外部系统触发时
	 * - 技能连锁或组合技激活时
	 */
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                                const FGameplayTagContainer* SourceTags = nullptr,
	                                const FGameplayTagContainer* TargetTags = nullptr,
	                                OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

protected:
	UAnimInstance* GetOwnerAnimInstance() const;

	/**
	 * @brief 从目标数据中获取球体扫描（Sphere Sweep）的碰撞结果
	 * @param TargetDataHandle 目标数据容器（包含起点和终点）
	 * @param SphereSweepRadius 球体扫描半径
	 * @param bDrawDebug 是否绘制调试可视化
	 * @param bIgnoreSelf 是否忽略施法者自身
	 * @return 返回所有命中的碰撞结果数组
	 */
	TArray<FHitResult> GetHitResultFromSweepLocationTargetData(const FGameplayAbilityTargetDataHandle& TargetDataHandle,
	                                                           float SphereSweepRadius = 30.f,
	                                                           ETeamAttitude::Type TargetTeam = ETeamAttitude::Hostile,
	                                                           bool bDrawDebug = false,
	                                                           bool bIgnoreSelf = true) const;
	FORCEINLINE bool ShouldDrawDebug() const { return bShouldDrawDebug; }

	/**
	 * @brief 对技能施法者自身施加推力效果
	 */
	void PushSelf(const FVector& PushVel);

	/**
	 * @brief 向指定目标施加推力（通过事件驱动）
	 * @param Target 被击飞的目标
	 * @param PushVel 击飞的速度
	 */
	void PushTarget(AActor* Target, const FVector& PushVel);
	void PushTargets(const TArray<AActor*>& Targets, const FVector& PushVel);
	void PushTargets(const FGameplayAbilityTargetDataHandle& TargetDataHandle, const FVector& PushVel);
	/**
	 * @brief 获取技能施法者的角色对象（带缓存优化）
	 * @return 
	 */
	ACharacter* GetOwningAvatarCharacter();

	/**
	 * @brief 应用伤害到目标(使用Effect)
	 * @param HitResult 击中的目标
	 * @param GameplayEffect 效果器
	 * @param Level 等级
	 */
	void ApplyGameplayEffectToHitResultActor(const FHitResult& HitResult, TSubclassOf<UGameplayEffect> GameplayEffect,
	                                         int Level = 1);

private:
	UPROPERTY(EditDefaultsOnly, Category="Debug")
	bool bShouldDrawDebug = false;

	// 技能施法者
	UPROPERTY()
	TObjectPtr<ACharacter> AvatarCharacter;
};
