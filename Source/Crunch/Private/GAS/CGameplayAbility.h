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
	                                                           float SphereSweepRadius = 30.f,ETeamAttitude::Type TargetTeam = ETeamAttitude::Hostile,
	                                                           bool bDrawDebug = false,
	                                                           bool bIgnoreSelf = true) const;
	FORCEINLINE bool ShouldDrawDebug() const {return bShouldDrawDebug;}

	/**
	 * @brief 对技能施法者自身施加推力效果
	 */
	void PushSelf(const FVector& PushVel);

	/**
	 * @brief 向指定目标施加推力（通过事件驱动）
	 * @param Target 被击飞的目标
	 * @param PushVel 击飞的速度
	 */
	void PushTarget(AActor* Target,const FVector& PushVel);
	/**
	 * @brief 获取技能施法者的角色对象（带缓存优化）
	 * @return 
	 */
	ACharacter* GetOwningAvatarCharacter() ;

	/**
	 * @brief 应用伤害到目标(使用Effect)
	 * @param HitResult 击中的目标
	 * @param GameplayEffect 效果器
	 * @param Level 等级
	 */
	void ApplyGameplayEffectToHitResultActor(const FHitResult& HitResult , TSubclassOf<UGameplayEffect> GameplayEffect,int Level = 1);
private:
	UPROPERTY(EditDefaultsOnly,Category="Debug")
	bool bShouldDrawDebug = false;

	// 技能施法者
	UPROPERTY()
	TObjectPtr<ACharacter> AvatarCharacter;
};
