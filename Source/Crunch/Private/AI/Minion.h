// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/CCharacter.h"
#include "Minion.generated.h"

/**
 * 
 */
UCLASS()
class AMinion : public ACCharacter
{
	GENERATED_BODY()
public:
	virtual void SetGenericTeamId(const FGenericTeamId& TeamID) override;

	/**
	 * @brief 检查小兵是否处于活动状态（非死亡状态）
	 * @return true=存活/可活动，false=死亡/不可活动
	 */
	bool IsActive()const;
	/**
	 * @brief 激活小兵（复活/启用）
	 */
	void Activate();
	/**
	 * @brief 设置目标
	 * @param Goal 
	 */
	void SetGoal(AActor* Goal);
private:
	/**
	 * @brief 根据当前队伍ID选择并应用对应的皮肤模型
	 */
	void PickSkinBasedOnTeamId();

	/**
	 * @brief 当队伍ID复制的时候调用
	 */
	virtual void OnRep_TeamId() override;
	// 目标点
	UPROPERTY(EditDefaultsOnly,Category = "AI")
	FName GoalBlackboardKeyName = "Goal";
	// 配置映射表：队伍ID与骨骼网格体的对应关系
	UPROPERTY(EditDefaultsOnly,Category = "Visual")
	TMap<FGenericTeamId,USkeletalMesh*>SkinMap;
	
};
