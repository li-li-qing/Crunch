// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GAP_Dead.h"
#include "GAS/CAbilitySystemStatics.h"
#include "Engine/OverlapResult.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GAS/CAttributeSet.h"
#include "GAS/CHeroAttributeSet.h"

UGAP_Dead::UGAP_Dead()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	// 创建技能触发数据配置
	FAbilityTriggerData TriggerData;
	// 设置触发源类型：GameplayEvent（游戏事件）
	// 表示此技能由游戏事件触发，而非玩家输入
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	// 设置触发标签：死亡状态标签
	// 当系统收到带有此标签的GameplayEvent时，自动激活此技能
	TriggerData.TriggerTag = UCAbilitySystemStatics::GetDeadStatTag();

	// 将触发配置添加到技能触发器列表
	// 一个技能可以有多个触发条件，这里添加死亡事件触发
	AbilityTriggers.Add(TriggerData);

	ActivationBlockedTags.RemoveTag(UCAbilitySystemStatics::GetStunStatTag());
}

void UGAP_Dead::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                const FGameplayAbilityActivationInfo ActivationInfo,
                                const FGameplayEventData* TriggerEventData)
{
	// 检查当前是否有网络权限（服务端执行）
	if (K2_HasAuthority())
	{
		// 从触发事件数据中获取击杀者信息
		// ContextHandle 包含 GameplayEffect 的上下文信息
		// GetEffectCauser() 返回导致效果发生的 Actor（击杀者）
		AActor* Killer = TriggerEventData->ContextHandle.GetEffectCauser();

		// 验证击杀者是否为有效英雄角色
		// 如果击杀者不存在或不是英雄，将 Killer 设为 nullptr
		if (!Killer || !UCAbilitySystemStatics::IsHero(Killer))
		{
			Killer = nullptr;
		}

		// 获取奖励目标列表（范围内的敌对英雄）
		TArray<AActor*> RewardTargets = GetRewardTargets();
        
		// 如果没有奖励目标且没有击杀者，直接结束技能
		if (RewardTargets.Num() == 0 && !Killer)
		{
			K2_EndAbility();
			return;
		}
        
		// 如果存在击杀者但不在目标列表中，将击杀者添加到奖励目标
		if (Killer && !RewardTargets.Contains(Killer))
		{
			RewardTargets.Add(Killer);
		}

		// 获取死亡角色的经验值属性
		bool bFound = false;
		float SelfExperience = GetAbilitySystemComponentFromActorInfo_Ensured()->GetGameplayAttributeValue(UCHeroAttributeSet::GetExperienceAttribute(), bFound);

		// 计算总奖励值
		// 总经验奖励 = 基础经验奖励 + 经验系数 × 自身经验值
		float TotalExperienceReward = BaseExperienceReward + ExperienceRewardPerExperience * SelfExperience;
        
		// 总金币奖励 = 基础金币奖励 + 金币系数 × 自身经验值
		float TotalGoldReward = BaseGoldReward + GoldRewardPerExperience * SelfExperience;

		if (!RewardEffectClass)return;
		// 如果存在击杀者，计算击杀者的奖励份额
		if (Killer)
		{
			// 击杀者获得总奖励的一部分（根据 KillerRewardPortion 比例）
			float KillerExperienceReward = TotalExperienceReward * KillerRewardPortion;
			float KillerGoldReward = TotalGoldReward * KillerRewardPortion;

			// 创建奖励效果规格，用于应用奖励到击杀者
			FGameplayEffectSpecHandle EffectSpec = MakeOutgoingGameplayEffectSpec(RewardEffectClass);
			if (EffectSpec.IsValid() && EffectSpec.Data.IsValid())
			{
				EffectSpec.Data->SetSetByCallerMagnitude(UCAbilitySystemStatics::GetExperienceAttributeTag(), KillerExperienceReward);
				EffectSpec.Data->SetSetByCallerMagnitude(UCAbilitySystemStatics::GetGoldAttributeTag(), KillerGoldReward);
                
				K2_ApplyGameplayEffectSpecToTarget(EffectSpec, UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(Killer));
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("GameplayEffectSpec创建失败，无法应用击杀者奖励"));
			}
			// 从总奖励中扣除击杀者获得的份额，得到剩余奖励
			TotalExperienceReward -= KillerExperienceReward;
			TotalGoldReward -= KillerGoldReward;
			
		}
		// 计算每个参与者获得的平均奖励
		float ExperiencePerTarget = TotalExperienceReward / RewardTargets.Num();
		float GoldPerTarget = TotalGoldReward / RewardTargets.Num();
		// 创建参与者奖励效果规格
		FGameplayEffectSpecHandle EffectSpec = MakeOutgoingGameplayEffectSpec(RewardEffectClass);
		// 设置参与者经验奖励
		EffectSpec.Data->SetSetByCallerMagnitude(UCAbilitySystemStatics::GetExperienceAttributeTag(),ExperiencePerTarget);
		// 设置参与者金币奖励
		EffectSpec.Data->SetSetByCallerMagnitude(UCAbilitySystemStatics::GetGoldAttributeTag(),GoldPerTarget);
		// 将奖励效果应用到所有参与者
		K2_ApplyGameplayEffectSpecToTarget(EffectSpec,UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActorArray(RewardTargets,true));
 
		// 结束技能
		K2_EndAbility();
	}
}

// 获取死亡奖励的目标列表：查找死亡角色周围可给予奖励的敌对英雄
TArray<AActor*> UGAP_Dead::GetRewardTargets() const
{
	// 使用TSet自动去重，避免重复目标
	TSet<AActor*> OutActors;

	// 获取当前技能的使用者（死亡的角色）
	AActor* AvatarActor = GetAvatarActorFromActorInfo();

	// 安全检查：确保角色和世界有效
	if (!AvatarActor || !GetWorld())
		return OutActors.Array();

	// 配置碰撞检测参数：只检测Pawn类型的物体
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);

	// 配置碰撞形状：球形检测区域
	FCollisionShape CollisionShape;
	CollisionShape.SetSphere(RewardRange);

	// 执行球形重叠检测，查找指定类型的所有物体
	// bool UWorld::OverlapMultiByObjectType(
	// 	TArray<FOverlapResult>& OutOverlaps,          // [OUT] 检测结果数组
	// 	const FVector& Pos,                           // [IN] 检测中心点（死亡角色位置）
	// 	const FQuat& Rot,                             // [IN] 旋转（通常为FQuat::Identity）
	// 	const FCollisionObjectQueryParams& ObjectParams, // [IN] 检测对象类型参数
	// 	const FCollisionShape& CollisionShape         // [IN] 碰撞形状（球形）
	// )
	// 作用：在死亡角色位置周围球形区域内检测所有Pawn
	// 执行球形重叠检测，查找范围内的所有Pawn
	TArray<FOverlapResult> OverlapResults;
	if (GetWorld()->OverlapMultiByObjectType(OverlapResults, AvatarActor->GetActorLocation(), FQuat::Identity,
	                                         ObjectQueryParams, CollisionShape))
	{
		// 遍历所有检测到的Actor
		for (const FOverlapResult& OverlapResult : OverlapResults)
		{
			// 检查团队关系：只选择敌对目标
			const IGenericTeamAgentInterface* OtherTeamAgentInterface = Cast<IGenericTeamAgentInterface>(
				OverlapResult.GetActor());

			// 跳过非敌对目标（友方、中立或没有团队接口的）
			if (!OtherTeamAgentInterface || OtherTeamAgentInterface->GetTeamAttitudeTowards(*AvatarActor) !=
				ETeamAttitude::Hostile)
			{
				continue;
			}

			// 检查目标类型：只选择英雄角色
			if (!UCAbilitySystemStatics::IsHero(OverlapResult.GetActor()))
			{
				continue;
			}

			// 通过所有筛选条件，添加到奖励目标列表
			OutActors.Add(OverlapResult.GetActor());
		}
	}

	// 将TSet转换为TArray返回
	return OutActors.Array();
}
