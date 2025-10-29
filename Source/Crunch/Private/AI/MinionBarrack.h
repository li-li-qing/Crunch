// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GenericTeamAgentInterface.h"
#include "MinionBarrack.generated.h"

class AMinion;
/**
 * @brief Minion这个AI生成的营房
 */
UCLASS()
class AMinionBarrack : public AActor
{
	GENERATED_BODY()
	
public:	

	AMinionBarrack();
	virtual void Tick(float DeltaTime) override;

protected:

	virtual void BeginPlay() override;

private:
	// 营地属于的队伍的ID
	UPROPERTY(EditAnywhere,Category = "Spawn")
	FGenericTeamId BarrackTeamId;
	// 小兵对象池  管理已生成的小兵引用，用于复用和状态追踪
	UPROPERTY()
	TArray<TObjectPtr<AMinion>> MinionPool;
	// 每组的小兵数量
	UPROPERTY(EditAnywhere,Category = "Spawn")
	int MinionPerGroup = 3;
	// 组生成的间隔时间
	UPROPERTY(EditAnywhere,Category = "Spawn")
	float GroupSpawnInterval = 5.f;
	// 设置目标点
	UPROPERTY(EditAnywhere,Category = "Spawn")
	AActor* Goal;
	// Minion小兵的类
	UPROPERTY(EditAnywhere,Category = "Spawn")
	TSubclassOf<AMinion> MinionClass;
	// 生成的位置
	UPROPERTY(EditAnywhere,Category = "Spawn")
	TArray<TObjectPtr< APlayerStart>> SpawnSpots;

	// 当我们需要生成一个新的小兵的时候,这个索引将递增并作为新小兵的标识符
	int NextSpawnSpotIndex = -1;
	/**
	 * @brief 获取下一个可用的生成点（循环选择）
	 * @return 返回下一个生成点的指针，无生成点时返回nullptr
	 */
	const APlayerStart* GetNextSpawnSpot();
	/**
	 * @brief 生成指定数量的小兵
	 * @param Amount 要生成的数量
	 */
	void SpawnNewMinions(int Amount);

	/**
	 * @brief 从对象池中获取下一个可用的（非活动）小兵  实现对象复用，避免频繁的生成/销毁操作
	 * @return 返回第一个找到的非活动小兵指针，无可用小兵时返回nullptr
	 */
	AMinion* GetNextAvaliableMinion();
	/**
	 * @brief 生成一组小兵（优先复用对象池中的小兵，不足时生成新小兵）
	 */
	UFUNCTION()
	void SpawnNewGroup();
	FTimerHandle SpawnIntervalTimerHandle;

	

};
