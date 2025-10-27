// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/CGameMode.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"

APlayerController* ACGameMode::SpawnPlayerController(ENetRole InRemoteRole, const FString& Options)
{
	// 调用父类方法生成基础PlayerController
	APlayerController* NewPlayerController =Super::SpawnPlayerController(InRemoteRole, Options);
	// 尝试转换为队伍接口（用于团队关系管理）
	IGenericTeamAgentInterface* NewPlayerTeamAgentInterface = Cast<IGenericTeamAgentInterface>(NewPlayerController);
	// 为玩家分配队伍ID（自动轮换红蓝两队）
	FGenericTeamId TeamId = GetTeamIdForPlayer(NewPlayerController);
	// 如果控制器实现了队伍接口，设置队伍ID
	if (NewPlayerTeamAgentInterface)
	{
		NewPlayerTeamAgentInterface->SetGenericTeamId(TeamId);
	}
	// 根据队伍ID分配出生点
	NewPlayerController->StartSpot = FindNextStartSpotForTeam(TeamId);

	return NewPlayerController;
}

FGenericTeamId ACGameMode::GetTeamIdForPlayer(const APlayerController* PlayerController) const
{
	static int PlayerCount = 0;// 静态计数器保持持久性
	++PlayerCount;
	return FGenericTeamId(PlayerCount % 2);// 轮换分配0或1
}

AActor* ACGameMode::FindNextStartSpotForTeam(const FGenericTeamId& TeamID) const
{
	// 从映射表中查找队伍对应的出生点标签
	const FName* StartSpotTag = TeamStartSpotTagMap.Find(TeamID);
	
	if (!StartSpotTag) return nullptr;

	UWorld* World = GetWorld();
	// 遍历场景中所有PlayerStart
	for (TActorIterator<APlayerStart> It(World) ; It;++It)
	{
		// 检查标签匹配且未被占用
		if (It->PlayerStartTag == *StartSpotTag)
		{
			// 标记为已占用
			It->PlayerStartTag = FName("Taken");
			return *It;
		}
	}
	return nullptr;
}
