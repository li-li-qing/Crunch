// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class UCAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	/**
	 * @brief 动画初始化原生重写点
	 *
	 * @details 当动画实例首次创建时调用，用于执行一次性初始化操作
	 *
	 * @note 此函数在动画蓝图实例化后立即调用，仅执行一次
	 * @warning 不要在此处执行耗时操作，会影响游戏启动性能
	 */
	 virtual void NativeInitializeAnimation() override;
	 /**
	  * @brief 动画更新原生重写点
	  *
	  * @param DeltaSeconds 自上一帧以来的时间（秒）
	  *
	  * @details 每帧调用，主要用于收集动画数据
	  * @note 建议在此仅收集数据，复杂计算应在NativeThreadSafeUpdateAnimation中进行
	  * @warning 在主游戏线程执行，避免耗时操作
	  */
	 virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	 /**
	  * @brief 线程安全的动画更新原生重写点
	  *
	  * @param DeltaSeconds 自上一帧以来的时间（秒）
	  *
	  * @details 在工作线程上执行，仅当关联的动画节点相关时调用
	  * @note 适合执行复杂动画计算，不会阻塞游戏线程
	  * @warning 不能访问非线程安全的UObject操作
	  */
	 virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

	 /**
	  * @brief 获取玩家速度
	  *
	  * @return 玩家速度
	  */
	 UFUNCTION(BlueprintCallable, Category = "Animation",meta=(BlueprintThreadSafe))
	 FORCEINLINE float GetSpeed() const { return Speed; }

	 /**
	  * @brief 是否在移动状态
	  * @return 玩家是否在移动
	  */
	 UFUNCTION(BlueprintCallable, Category = "Animation",meta=(BlueprintThreadSafe))
	 FORCEINLINE bool IsMoving() const { return Speed != 0.f; }

	  /**
	  * @brief 是否在静止状态
	  * @return 玩家是否在静止
	  */
	 UFUNCTION(BlueprintCallable, Category = "Animation",meta=(BlueprintThreadSafe))
	 FORCEINLINE bool IsNotMoving() const { return Speed == 0.f; }

private:
	// 获取玩家角色
	UPROPERTY()
	class ACharacter* OwnerCharacter;

	// 获得玩家角色的移动组件
	UPROPERTY()
	class UCharacterMovementComponent* OwnerCharacterMovementComponent;

	// 玩家的移动速度
	float Speed;
};
