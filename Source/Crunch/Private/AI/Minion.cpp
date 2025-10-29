// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Minion.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

void AMinion::SetGenericTeamId(const FGenericTeamId& TeamID)
{
	Super::SetGenericTeamId(TeamID);
	PickSkinBasedOnTeamId();
}

bool AMinion::IsActive() const
{
	// 不是死亡状态
	return !IsDead();
}

void AMinion::Activate()
{
	// 移除死亡标签
	RespawnImmediately();
}

void AMinion::SetGoal(AActor* Goal)
{
	// 获得AI控制器
	if (AAIController* AIController = GetController<AAIController>())
	{
		// 获得这个AI控制器下的黑板
		if (UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent())
		{
			// 设置键的值
			BlackboardComponent->SetValueAsObject(GoalBlackboardKeyName, Goal);
		}
	}
}

void AMinion::PickSkinBasedOnTeamId()
{
	// 在SkinMap映射表中查找当前队伍ID对应的骨骼网格体
	USkeletalMesh** Skin = SkinMap.Find(GetGenericTeamId());
	if (Skin)
	{
		// 将找到的骨骼网格体应用到角色的网格组件
		GetMesh()->SetSkeletalMesh(*Skin);
	}
}

void AMinion::OnRep_TeamId()
{
	PickSkinBasedOnTeamId();
}
