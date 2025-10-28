// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/CAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Character/CCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GAS/CAbilitySystemStatics.h"

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
	SightConfig->SightRadius = 1000.f; // 最大检测距离（厘米）
	SightConfig->LoseSightRadius = 1200.f; // 丢失视野距离（需略大于SightRadius）
	// 设置感知信息有效期（秒） 超过此时间未检测到目标，将视为目标消失
	SightConfig->SetMaxAge(5.f);
	// 设置周边视野角度（总角度） 180度表示正前方半球形视野
	SightConfig->PeripheralVisionAngleDegrees = 180.f;
	// 将视觉配置注册到感知组件
	AIPerceptionComponent->ConfigureSense(*SightConfig);
	// AI感知系统检测到目标变化 时（如发现玩家、丢失目标、听到声音），会自动调用绑定的 TargetPerceptionUpdated函数。
	AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ACAIController::TargetPerceptionUpdated);
	// 在AI完全遗忘某个目标时
	AIPerceptionComponent->OnTargetPerceptionForgotten.AddDynamic(this, &ACAIController::TargetForgotten);
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

void ACAIController::BeginPlay()
{
	Super::BeginPlay();
	// 运行行为树
	RunBehaviorTree(BehaviorTree);
}

void ACAIController::TargetPerceptionUpdated(AActor* TargetActor, FAIStimulus Stimulus)
{
	// 检查是否为有效感知（目标进入视野/听觉范围等）
	if (Stimulus.WasSuccessfullySensed())
	{
		// 如果当前没有锁定目标，则设置新目标
		if (!GetCurrentTarget())
		{
			SetCurrentTarget(TargetActor);
		}
	}
	else // 目标丢失（离开视野或超出感知范围）
	{
		ForgetActorIfDead(TargetActor);
	}
}

void ACAIController::TargetForgotten(AActor* ForgottenActor)
{
	if (!ForgottenActor)return;

	// 如果当前看向的目标等于要遗忘的目标,我们就看向下一个目标
	if (GetCurrentTarget() == ForgottenActor)
	{
		SetCurrentTarget(GetNextPerceivedActor());
	}
}

AActor* ACAIController::GetNextPerceivedActor() const
{
	// 获得所有被察觉的Actor
	TArray<AActor*> PerceivedActors;
	AIPerceptionComponent->GetPerceivedHostileActors(PerceivedActors);
	// 如果有数据,返回第一个就行了
	if (PerceivedActors.Num() != 0) return PerceivedActors[0];
	return nullptr;
}

void ACAIController::ForgetActorIfDead(AActor* ActorToForget)
{
	// 获取目标Actor的GAS组件（用于检查死亡状态）
	const UAbilitySystemComponent* ActorASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(ActorToForget);
	if (!ActorASC)return;

	// 检查目标是否带有"Dead"标签
	if (ActorASC->HasMatchingGameplayTag(UCAbilitySystemStatics::GetDeadStatTag()))
	{
		// 遍历AI感知系统中对该Actor的所有感知记录
		for (UAIPerceptionComponent::TActorPerceptionContainer::TIterator Iter = AIPerceptionComponent->
			     GetPerceptualDataIterator(); Iter; ++Iter)
		{
			// 跳过非目标Actor的记录
			if (Iter->Key != ActorToForget)
			{
				continue;
			}
			// 修改该Actor的所有感知刺激数据
			for (FAIStimulus& Stimuli : Iter->Value.LastSensedStimuli)
			{
				// 距离最后一次被感知的时间设置为最大值
				Stimuli.SetStimulusAge(TNumericLimits<float>::Max());
			}
		}
	}
}

const UObject* ACAIController::GetCurrentTarget() const
{
	const UBlackboardComponent* BlackboardComponent = GetBlackboardComponent();
	if (BlackboardComponent)
	{
		// 通过黑板组件获取键值对
		return BlackboardComponent->GetValueAsObject(TargetBlackboardKeyName);
	}
	return nullptr;
}

void ACAIController::SetCurrentTarget(AActor* NewTarget)
{
	UBlackboardComponent* BlackboardComponent = GetBlackboardComponent();
	if (!BlackboardComponent) return;
	// 设置新目标
	if (NewTarget)
	{
		// 将目标Actor存入黑板
		BlackboardComponent->SetValueAsObject(TargetBlackboardKeyName, NewTarget);
	}
	else // 清除目标
	{
		BlackboardComponent->ClearValue(TargetBlackboardKeyName);
	}
}
