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
};
