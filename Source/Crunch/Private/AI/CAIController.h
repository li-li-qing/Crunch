// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CAIController.generated.h"
class UAISenseConfig_Sight;
class UAIPerceptionComponent;
class UBehaviorTree;
/**
 * AI控制器
 */
UCLASS()
class ACAIController : public AAIController
{
	GENERATED_BODY()
public:
	ACAIController();
	virtual void OnPossess(APawn* Possessed)override;
	virtual void BeginPlay()override;
private:
	// 黑板Target 这个key的名字
	UPROPERTY(EditDefaultsOnly,Category="AI Behavior")
	FName TargetBlackboardKeyName = "Target";
	// AI行为树
	UPROPERTY(EditDefaultsOnly,Category="AI Behavior")
	TObjectPtr<UBehaviorTree> BehaviorTree;
	// AI感知组件
	UPROPERTY(VisibleDefaultsOnly,Category="Perception")
	TObjectPtr<UAIPerceptionComponent> AIPerceptionComponent;
	// 视觉感知配置
	UPROPERTY(VisibleDefaultsOnly,Category="Perception")
	TObjectPtr<UAISenseConfig_Sight> SightConfig;

	/**
	 * @brief AI感知目标更新的回调函数
	 * @param TargetActor 被感知到的目标Actor（如玩家角色）
	 * @param Stimulus 感知刺激数据（包含感知类型、强度等信息）
	 */
	UFUNCTION()
	void TargetPerceptionUpdated(AActor* TargetActor,FAIStimulus Stimulus);

	/**
	 * @brief 从黑板中获取当前锁定的目标
	 * @return 返回当前目标对象指针，无目标时返回nullptr
	 */
	const UObject* GetCurrentTarget() const;
	/**
	 * @brief 更新或清除当前目标
	 * @param NewTarget 要设置的新目标Actor指针，传nullptr表示清除目标
	 */
	void SetCurrentTarget(AActor* NewTarget);

	/**
	 * @brief 在AI完全遗忘某个目标时的回调函数
	 * @param ForgottenActor 
	 */
	UFUNCTION()
	void TargetForgotten(AActor* ForgottenActor);

	/**
	 * @brief 获得下一个被察觉的Actor
	 * @return 
	 */
	AActor* GetNextPerceivedActor() const;

	/**
	 * @brief 检查目标Actor是否死亡，若死亡则强制AI遗忘该目标
	 * @param ActorToForget 需要检查的目标Actor（通常是玩家或敌人）
	 */
	void ForgetActorIfDead(AActor* ActorToForget);
};
