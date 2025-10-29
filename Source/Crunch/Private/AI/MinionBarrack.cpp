// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/MinionBarrack.h"
#include "AI/Minion.h"
#include "GameFramework/PlayerStart.h"

// Sets default values
AMinionBarrack::AMinionBarrack()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMinionBarrack::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		// 设置定时器
		GetWorldTimerManager().SetTimer(SpawnIntervalTimerHandle, this, &AMinionBarrack::SpawnNewGroup,GroupSpawnInterval,true);
	}
}

const APlayerStart* AMinionBarrack::GetNextSpawnSpot()
{
	// 如果生成的位置数量等于0,直接退出
	if (SpawnSpots.Num() == 0)
	{
		return nullptr;
	}
	// 递增索引指向下一个生成点
	++NextSpawnSpotIndex;
	// 循环检查：如果索引超出范围，重置为0（实现循环选择）
	if (NextSpawnSpotIndex >= SpawnSpots.Num())
	{
		NextSpawnSpotIndex = 0;
	}
	// 返回当前索引对应的生成点
	return SpawnSpots[NextSpawnSpotIndex];
}

void AMinionBarrack::SpawnNewMinions(int Amount)
{
	// 循环生成指定数量的小兵
	for (int i = 0; i < Amount; i++)
	{
		// 设置默认生成变换（使用兵营自身位置）
		FTransform SpawnTransform = GetActorTransform();
		// 获取下一个生成点并应用其变换
		if (const APlayerStart* NextSpawnSpot = GetNextSpawnSpot())
		{
			SpawnTransform = NextSpawnSpot->GetActorTransform();
		}
		// 使用延迟生成方式创建小兵
		AMinion* NewMinion = GetWorld()->SpawnActorDeferred<AMinion>(MinionClass, SpawnTransform, this, nullptr,
		                                                             ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
		// 设置小兵的队伍归属
		NewMinion->SetGenericTeamId(BarrackTeamId);
		// 用于在配置完所有属性后正式激活
		NewMinion->FinishSpawning(SpawnTransform);
		// 设置目标点
		NewMinion->SetGoal(Goal);
		
		// 将新生成的小兵添加到对象池
		MinionPool.Add(NewMinion);
	}
}

AMinion* AMinionBarrack::GetNextAvaliableMinion()
{
	// 遍历小兵对象池中的所有小兵
	for (AMinion* Minion : MinionPool)
	{
		// 检查小兵是否处于非活动状态（如死亡、休眠等）
		if (!Minion->IsActive())
		{
			// 找到可复用的小兵，立即返回
			return Minion;
		}
	}
	return nullptr;
}

void AMinionBarrack::SpawnNewGroup()
{
	// 初始化计数器：需要生成的小兵数量
	int i = MinionPerGroup;
	// 优先尝试复用对象池中的非活动小兵
	while (i > 0)
	{
		// 准备生成位置：使用兵营自身位置作为默认值
		FTransform SpawnTransform = GetActorTransform();
		// 获取下一个生成点（如果存在则使用生成点的位置）
		if (const APlayerStart* NextSpawnSpot = GetNextSpawnSpot())
		{
			SpawnTransform = NextSpawnSpot->GetActorTransform();
		}
		// 从对象池中获取下一个可复用的小兵
		AMinion* NextAvaliableMinion = GetNextAvaliableMinion();
		// 检查是否找到可用小兵
		if (!NextAvaliableMinion)
		{
			break;
		}
		// 设置小兵的位置和旋转
		NextAvaliableMinion->SetActorTransform(SpawnTransform);
		// 激活小兵（清除死亡状态、恢复属性等）
		NextAvaliableMinion->Activate();
		// 减少待生成数量
		--i;
	}
	// 对剩余需要的小兵数量，生成全新的小兵
	SpawnNewMinions(i);
}

// Called every frame
void AMinionBarrack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
