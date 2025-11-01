// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/UpperCut.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "GAS/GA_Combo.h"
#include "GameplayTagsManager.h"
#include "GAS/CAbilitySystemStatics.h"

UUpperCut::UUpperCut()
{
}

void UUpperCut::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                const FGameplayAbilityActivationInfo ActivationInfo,
                                const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// 尝试提交技能（资源检查）
	if (!K2_CommitAbility())
	{
		K2_EndAbility();
		return;
	}
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		UAbilityTask_PlayMontageAndWait* PlayUpperCutMontageTask =
			UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, UpperCutMontage);
		PlayUpperCutMontageTask->OnBlendOut.AddDynamic(this, &UUpperCut::K2_EndAbility);
		PlayUpperCutMontageTask->OnCancelled.AddDynamic(this, &UUpperCut::K2_EndAbility);
		PlayUpperCutMontageTask->OnInterrupted.AddDynamic(this, &UUpperCut::K2_EndAbility);
		PlayUpperCutMontageTask->OnCompleted.AddDynamic(this, &UUpperCut::K2_EndAbility);
		PlayUpperCutMontageTask->ReadyForActivation();
		// 等待上勾拳的标签
		UAbilityTask_WaitGameplayEvent* WaitLaunchEventTag = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this, GetUpperCutLaunchTag());
		WaitLaunchEventTag->EventReceived.AddDynamic(this, &UUpperCut::StartLaunching);
		WaitLaunchEventTag->ReadyForActivation();
	}
	NextComboName = NAME_None;
}


const FGenericDamageEffectDef* UUpperCut::GetDamageEffectDerForCurrentCombo() const
{
	// ► 1. 获取技能施法者的动画实例
	UAnimInstance* OwnerAnimInstance = GetOwnerAnimInstance();
    
	// ► 2. 验证动画实例有效性
	if (OwnerAnimInstance)
	{
		// ► 3. 获取当前正在播放的动画片段名称
		FName CurrentComboName = OwnerAnimInstance->Montage_GetCurrentSection(UpperCutMontage);
        
		// ► 4. 在伤害映射表中查找对应的伤害效果配置
		const FGenericDamageEffectDef* EffectDef = ComboDamageMap.Find(CurrentComboName);
        
		// ► 5. 返回找到的效果定义（可能为nullptr）
		return EffectDef;
	}
    
	// ► 6. 动画实例无效时返回空指针
	return nullptr;
}

FGameplayTag UUpperCut::GetUpperCutLaunchTag()
{
	return FGameplayTag::RequestGameplayTag("Ability.UpperCut.Launch");
}

void UUpperCut::StartLaunching(FGameplayEventData EventData)
{
	TArray<FHitResult> TargetHitResults = GetHitResultFromSweepLocationTargetData(
		EventData.TargetData, TargetSweepSphereRadius, ETeamAttitude::Hostile, ShouldDrawDebug());
	if (K2_HasAuthority())
	{
		// 击飞自己
		PushTarget(GetAvatarActorFromActorInfo(), FVector::UpVector * UpperCutLaunchSpeed);
		for (FHitResult HitResult : TargetHitResults)
		{
			// 击飞每个命中的敌人
			PushTarget(HitResult.GetActor(), FVector::UpVector * UpperCutLaunchSpeed);
			ApplyGameplayEffectToHitResultActor(HitResult, LaunchDamageEffect,
			                                    GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
		}
	}
	UAbilityTask_WaitGameplayEvent* WaitComboChangeEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, UGA_Combo::GetCombaChangedEventTag(), nullptr, false, false);
	WaitComboChangeEvent->EventReceived.AddDynamic(this, &UUpperCut::HandleComboChangeEvent);
	WaitComboChangeEvent->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* WaitComboCommitEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, UCAbilitySystemStatics::GetBasicAttackInputPressedTag());
	WaitComboCommitEvent->EventReceived.AddDynamic(this, &UUpperCut::HandleComboCommitEvent);
	WaitComboCommitEvent->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* WaitComboDamageEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, UGA_Combo::GetComboTargetEventTag());
	WaitComboDamageEvent->EventReceived.AddDynamic(this, &UUpperCut::HandleComboDamageEvent);
	WaitComboDamageEvent->ReadyForActivation();
}

void UUpperCut::HandleComboDamageEvent(FGameplayEventData EventData)
{
	// 上勾拳连击的伤害
	if (K2_HasAuthority())
	{
		TArray<FHitResult> TargetHitResults = GetHitResultFromSweepLocationTargetData(
			EventData.TargetData, TargetSweepSphereRadius, ETeamAttitude::Hostile, ShouldDrawDebug());
		PushTarget(GetAvatarActorFromActorInfo(), FVector::UpVector * UpperComboHoldSpeed);
		const FGenericDamageEffectDef* EffectDef = GetDamageEffectDerForCurrentCombo();
		if (!EffectDef) return;
		for (FHitResult& HitResult : TargetHitResults)
		{
			FVector PushVel = GetAvatarActorFromActorInfo()->GetActorTransform().TransformVector(EffectDef->PushVelocity);
			PushTarget(HitResult.GetActor(), PushVel);
			ApplyGameplayEffectToHitResultActor(HitResult, EffectDef->DamageEffect,
			                                    GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
		}
	}
}

void UUpperCut::HandleComboChangeEvent(FGameplayEventData EventData)
{
	// 提取事件标签（标识具体的事件类型）
	FGameplayTag EventTag = EventData.EventTag;
	// 检查是否为连招结束事件
	if (EventTag == UGA_Combo::GetCombaChangedEventEndTag())
	{
		// 连招结束：清空下一个连招名称
		NextComboName = NAME_None;
		UE_LOG(LogTemp,Warning,TEXT("Next Combo is :%s"),*NextComboName.ToString());
		return;
	}
	// 将GameplayTag拆分为FName数组（按层级分割）
	TArray<FName> TagNames;
	UGameplayTagsManager::Get().SplitGameplayTagFName(EventTag, TagNames);

	// 取最后一个分段作为下一个连招名称
	NextComboName = TagNames.Last();
	UE_LOG(LogTemp,Warning,TEXT("Next Combo is :%s"),*NextComboName.ToString());
}

void UUpperCut::HandleComboCommitEvent(FGameplayEventData EventData)
{
	UE_LOG(LogTemp,Warning,TEXT("Combo Change Commit"));
	if (NextComboName == NAME_None)
	{
		return;
	}
	UAnimInstance* OwnerAnimInstance = GetOwnerAnimInstance();
	if (!OwnerAnimInstance) return;

	OwnerAnimInstance->Montage_SetNextSection(OwnerAnimInstance->Montage_GetCurrentSection(UpperCutMontage),
	                                          NextComboName, UpperCutMontage);
}
