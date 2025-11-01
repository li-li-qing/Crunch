// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GAP_Launch.h"

#include "CAbilitySystemStatics.h"

UGAP_Launch::UGAP_Launch()
{
	// 设置网络执行策略为"仅服务端"
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;

	// 配置技能触发方式
	FAbilityTriggerData TriggerData;
	// 由事件驱动
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	// 获得起飞的标签
	TriggerData.TriggerTag = GetLauchedAbilityActivationTag();
	// 移除阻挡标签 "眩晕"
	ActivationBlockedTags.RemoveTag(UCAbilitySystemStatics::GetStunStatTag());
	// 添加到技能触发器列表
	AbilityTriggers.Add(TriggerData);
}

void UGAP_Launch::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!K2_CommitAbility())
	{
		K2_EndAbility();
		return;
	}
	if (K2_HasAuthority())
	{
		// 从事件数据获取推力方向并应用
		PushSelf(TriggerEventData->TargetData.Get(0)->GetHitResult()->ImpactNormal);
		K2_EndAbility();
	}
}

FGameplayTag UGAP_Launch::GetLauchedAbilityActivationTag()
{
	return FGameplayTag::RequestGameplayTag("Ability.UpperCut.Launch.Activate");
}
