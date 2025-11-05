// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "TargetActor_GroundPick.generated.h"
class UDecalComponent;
/**
 * 
 */
UCLASS()
class ATargetActor_GroundPick : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()

public:
	ATargetActor_GroundPick();

	/**
	 * @brief 确认当前目标选择并继续执行技能逻辑
	 * @details 在目标选择过程中，当玩家确认选择时调用
	 */
	virtual void ConfirmTargetingAndContinue() override;
	/**
	 * 设置目标检测区域的半径
	 * @param NewRadius 新的检测半径
	 */
	void SetTargetAreaRadius(float NewRadius);

	
	FORCEINLINE void SetTargetTraceRange(float NewRange){TargetTraceRange = NewRange;}
	/**
	 * 设置目标选择选项：是否目标友方/敌方单位
	 * @param bTargetFriendly 是否目标友方单位
	 * @param bTargetEnemy 是否目标敌方单位
	 */
	void SetTargetOptions(bool bTargetFriendly,bool bTargetEnemy = true);

	FORCEINLINE void SetShouldDrawDebug(bool bDrawDebug){bShouldDrawDebug = bDrawDebug;}
protected:
	virtual void Tick(float DeltaSeconds) override;

	bool bShouldTargetEnemy = true;
	bool bShouldTargetFriendly = false;
	/**
	 * 计算目标点位置（通过射线检测确定地面交点）
	 * @return 计算出的目标点世界坐标
	 */
	FVector GetTargetPoint() const;
	// 目标区域半径
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetAreaRadius = 300.f;
	// 检测的长度
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetTraceRange = 2000.f;

	bool bShouldDrawDebug = false;

private:
	UPROPERTY(EditDefaultsOnly,Category="Visual")
	TObjectPtr<UDecalComponent> DecalComp;

};
