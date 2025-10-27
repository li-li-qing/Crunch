// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/CAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Character/CCharacter.h"
ACAIController::ACAIController()
{
	//创建AI感知组件（核心感知处理器）
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AI Perception Component"));
	// 创建视觉感知配置（定义视野规则）
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	// 配置检测目标类型（基于队伍关系）
	SightConfig->DetectionByAffiliation.bDetectEnemies = true; // 检测敌方
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false; // 不检测友方
	SightConfig->DetectionByAffiliation.bDetectNeutrals = false; // 不检测中立
	// 设置视觉范围参数
	SightConfig->SightRadius = 1000.f;	// 最大检测距离（厘米）
	SightConfig->LoseSightRadius = 1200.f;	// 丢失视野距离（需略大于SightRadius）
	// 设置感知信息有效期（秒） 超过此时间未检测到目标，将视为目标消失
	SightConfig->SetMaxAge(5.f);
	// 设置周边视野角度（总角度） 180度表示正前方半球形视野
	SightConfig->PeripheralVisionAngleDegrees = 180.f;
	// 将视觉配置注册到感知组件
	AIPerceptionComponent->ConfigureSense(*SightConfig);
}

void ACAIController::OnPossess(APawn* Possessed)
{
	Super::OnPossess(Possessed);
	// 为AI控制器设置队伍ID（0表示默认队伍）
	SetGenericTeamId(FGenericTeamId(0));
	// 检查被控制的Pawn是否实现了团队接口
	IGenericTeamAgentInterface* PawnTeamAgentInterface = Cast<IGenericTeamAgentInterface>(Possessed);
	// 如果Pawn支持团队接口，同步队伍ID
	if (PawnTeamAgentInterface)
	{
		// 将Pawn的队伍ID设置为与控制器的队伍ID一致
		PawnTeamAgentInterface->SetGenericTeamId(GetGenericTeamId());
	}
	
}
