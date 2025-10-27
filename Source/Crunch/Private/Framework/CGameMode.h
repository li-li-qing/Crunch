// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GenericTeamAgentInterface.h"
#include "CGameMode.generated.h"

/**
 * 
 */
UCLASS()
class ACGameMode : public AGameModeBase 
{
	GENERATED_BODY()

	/**
	 * @brief 生成玩家控制器并初始化队伍信息
	 * @param InRemoteRole 网络角色（用于确定权限）
	 * @param Options 附加选项字符串（如URL参数）
	 * @return 
	 */
	virtual APlayerController* SpawnPlayerController(ENetRole InRemoteRole, const FString& Options) override;

private:
	/**
	 * @brief 为玩家分配队伍ID（简单轮换红蓝两队）
	 * @param PlayerController 需要分配队伍的玩家控制器
	 * @return 队伍ID（0=红队，1=蓝队）
	 */
	FGenericTeamId GetTeamIdForPlayer(const APlayerController* PlayerController) const ;

	/**
	 * @brief 根据队伍ID查找对应的出生点
	 * @param TeamID 队伍标识符
	 * @return 匹配的出生点Actor，找不到返回nullptr
	 */
	AActor* FindNextStartSpotForTeam(const FGenericTeamId& TeamID)const;

	UPROPERTY(EditDefaultsOnly,Category = "Team")
	TMap<FGenericTeamId,FName> TeamStartSpotTagMap;
};
