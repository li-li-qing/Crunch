// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA_Combo.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "GAS/CAbilitySystemStatics.h"
#include "GameplayTagsManager.h"
#include "AbilitySystemBlueprintLibrary.h"

UGA_Combo::UGA_Combo()
{
	// 给自己这个能力添加标签
	AbilityTags.AddTag(UCAbilitySystemStatics::GetBasicAttackAbilityTag());
	// 如果标签里面有这个则无法执行
	BlockAbilitiesWithTag.AddTag(UCAbilitySystemStatics::GetBasicAttackAbilityTag());
}

void UGA_Combo::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                const FGameplayAbilityActivationInfo ActivationInfo,
                                const FGameplayEventData* TriggerEventData)
{
	// 尝试提交技能（检查资源消耗、冷却时间等条件）
	if (!K2_CommitAbility()) //蓝图兼容的CommitAbility封装
	{
		//如果提交失败（如蓝量不足、冷却中），立即终止技能
		K2_EndAbility(); // 蓝图兼容的EndAbility封装
		return;
	}
	// 检查网络权限：仅在服务器或预测执行
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		//创建并激活蒙太奇播放任务
		UAbilityTask_PlayMontageAndWait* PlayComboMontageTask =
			UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
				this, // 任务所属的Ability实例
				NAME_None, // 任务名称（可自定义）
				ComboMontage // 要播放的蒙太奇资源
			);
		// 绑定蒙太奇事件回调
		PlayComboMontageTask->OnBlendOut.AddDynamic(this, &UGA_Combo::K2_EndAbility); // 动画混合结束时
		PlayComboMontageTask->OnCancelled.AddDynamic(this, &UGA_Combo::K2_EndAbility); // 动画被取消时
		PlayComboMontageTask->OnCompleted.AddDynamic(this, &UGA_Combo::K2_EndAbility); // 动画正常完成时
		PlayComboMontageTask->OnInterrupted.AddDynamic(this, &UGA_Combo::K2_EndAbility); // 动画被中断时
		// 激活任务（开始播放蒙太奇）
		PlayComboMontageTask->ReadyForActivation();

		UAbilityTask_WaitGameplayEvent* WaitGameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this, //拥有该Task的Ability实例（通常是当前技能）
			GetCombaChangedEventTag(), //要监听的事件标签（如"Ability.Combo.Change"）
			nullptr, //可选的外部目标Actor（null表示监听自己）
			false, //是否仅触发一次（one-shot）
			false //是否监听相同标签的子标签（如"Ability.Combo.Change.Damage"）模糊查找
		);
		WaitGameplayEventTask->EventReceived.AddDynamic(this, &UGA_Combo::ComboChangedEventReceived);
		// 激活Task，开始监听事件
		WaitGameplayEventTask->ReadyForActivation();
	}
	if (K2_HasAuthority())
	{
		// 等待游戏事件
		UAbilityTask_WaitGameplayEvent* WaitTargetingEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this, GetComboTargetEventTag());
		// 如果抓取到了这个事件,执行DoDamage
		WaitTargetingEventTask->EventReceived.AddDynamic(this, &UGA_Combo::DoDamage);
		// 激活Task，开始监听事件
		WaitTargetingEventTask->ReadyForActivation();
	}

	// 开始监听玩家输入
	SetupWaitComboInputPress();
}

FGameplayTag UGA_Combo::GetCombaChangedEventTag()
{
	return FGameplayTag::RequestGameplayTag("Ability.Combo.Change");
}

FGameplayTag UGA_Combo::GetCombaChangedEventEndTag()
{
	return FGameplayTag::RequestGameplayTag("Ability.Combo.Change.End");
}

FGameplayTag UGA_Combo::GetComboTargetEventTag()
{
	return FGameplayTag::RequestGameplayTag("Ability.Combo.Damage");
}

void UGA_Combo::ComboChangedEventReceived(FGameplayEventData Data)
{
	// 提取事件标签
	FGameplayTag EventTag = Data.EventTag;
	// 情况1：收到结束信号
	if (EventTag == GetCombaChangedEventEndTag())
	{
		NextComboName = NAME_None; // 清除下一段连招
		//UE_LOG(LogTemp,Warning,TEXT("Next Combo is cleared"));
		return;
	}
	//解析标签层级（如"Ability.Combo.Change.Combo01" -> ["Ability","Combo","Change","Combo01"]）
	TArray<FName> TagNames;
	UGameplayTagsManager::Get().SplitGameplayTagFName(EventTag, TagNames);
	// 取最后一级作为下一连招名称（如"Combo01"）
	NextComboName = TagNames.Last();
 
	//UE_LOG(LogTemp,Warning,TEXT("Next combo is now : %s"),*NextComboName.ToString());
}

void UGA_Combo::SetupWaitComboInputPress()
{
	// 创建等待输入按下的AbilityTask
	UAbilityTask_WaitInputPress* WaitInputPress = UAbilityTask_WaitInputPress::WaitInputPress(this);
	// 绑定输入按下事件的回调函数
	WaitInputPress->OnPress.AddDynamic(this, &UGA_Combo::HandleInputPress);
	// 激活任务，开始监听输入
	WaitInputPress->ReadyForActivation();
}

void UGA_Combo::HandleInputPress(float TimeWaited)
{
	SetupWaitComboInputPress();
	TryCommitCombo();
}

void UGA_Combo::DoDamage(FGameplayEventData Data)
{

	// 从TargetData中获取球体扫描的命中结果
	TArray<FHitResult> HitResults = GetHitResultFromSweepLocationTargetData(
		Data.TargetData, TargetSweepSphereRadius);
	
	// 遍历所有命中的目标
	for (const FHitResult& HitResult : HitResults)
	{
		// 获取当前连招段对应的GameplayEffect
		TSubclassOf<UGameplayEffect> GameplayEffect = GetDamageEffectForCurrentCombo();
		ApplyGameplayEffectToHitResultActor(HitResult,GameplayEffect,GetAbilityLevel(CurrentSpecHandle,CurrentActorInfo));
	}
	
}

void UGA_Combo::TryCommitCombo()
{
	if (NextComboName == NAME_None)return;

	UAnimInstance* OwnerAnimInstance = GetOwnerAnimInstance();
	if (!OwnerAnimInstance)return;

	// 动态修改蒙太奇当前段→下一段
	OwnerAnimInstance->Montage_SetNextSection(OwnerAnimInstance->Montage_GetCurrentSection(
		ComboMontage),
		NextComboName,
		ComboMontage
		);
}

TSubclassOf<UGameplayEffect> UGA_Combo::GetDamageEffectForCurrentCombo() const
{
	// 1. 获取技能施法者的动画实例
	UAnimInstance* OwnerAnimInstance = GetOwnerAnimInstance();

	// 2. 检查动画实例是否有效
	if (OwnerAnimInstance)
	{
		// 3. 获取当前正在播放的蒙太奇片段名称
		// 示例：如果当前播放的是"Attack1"片段，返回"Attack1"
		FName CurrentSectionName = OwnerAnimInstance->Montage_GetCurrentSection(ComboMontage);

		// 4. 在伤害效果映射表中查找对应的GameplayEffect类
		// DamageEffectMap 应该是 TMap<FName, TSubclassOf<UGameplayEffect>> 类型
		const TSubclassOf<UGameplayEffect>* FoundEffectPtr = DamageEffectMap.Find(CurrentSectionName);

		// 5. 如果找到对应的效果类，返回它
		if (FoundEffectPtr)
		{
			return *FoundEffectPtr;
		}
	}
	return DefaultDamageEffect;
}
