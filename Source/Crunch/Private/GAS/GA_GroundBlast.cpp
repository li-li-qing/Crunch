// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA_GroundBlast.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GAS/CAbilitySystemStatics.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "GAS/TargetActor_GroundPick.h"
#include "Player/CPlayerCharacter.h"

UGA_GroundBlast::UGA_GroundBlast()
{
	ActivationOwnedTags.AddTag(UCAbilitySystemStatics::GetAimStatTag());

	BlockAbilitiesWithTag.AddTag(UCAbilitySystemStatics::GetBasicAttackAbilityTag());
}

void UGA_GroundBlast::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                      const FGameplayAbilityActorInfo* ActorInfo,
                                      const FGameplayAbilityActivationInfo ActivationInfo,
                                      const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!HasAuthorityOrPredictionKey(CurrentActorInfo, &CurrentActivationInfo)) return;

	UAbilityTask_PlayMontageAndWait* PlayGroundBlastAnimTask =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, TargettingMontage);
	PlayGroundBlastAnimTask->OnBlendOut.AddDynamic(this, &UGA_GroundBlast::K2_EndAbility);
	PlayGroundBlastAnimTask->OnCancelled.AddDynamic(this, &UGA_GroundBlast::K2_EndAbility);
	PlayGroundBlastAnimTask->OnCompleted.AddDynamic(this, &UGA_GroundBlast::K2_EndAbility);
	PlayGroundBlastAnimTask->OnInterrupted.AddDynamic(this, &UGA_GroundBlast::K2_EndAbility);
	PlayGroundBlastAnimTask->ReadyForActivation();

	// 创建并配置目标选择等待任务
	// AbilityTask_WaitTargetData：等待玩家选择目标的任务
	UAbilityTask_WaitTargetData* WaitTargetDataTask = UAbilityTask_WaitTargetData::WaitTargetData(
		this,                                           // 拥有该Task的GameplayAbility
		NAME_None,                                      // 任务实例名称
		EGameplayTargetingConfirmation::UserConfirmed,   // 确认方式：需要用户确认（如鼠标点击）
		TargetActorClass                                // 目标选择器类（用于可视化目标选择）
	);
    
	// 绑定目标选择成功回调
	WaitTargetDataTask->ValidData.AddDynamic(this, &UGA_GroundBlast::TargetConfirmed);
	// 绑定目标选择取消回调
	WaitTargetDataTask->Cancelled.AddDynamic(this, &UGA_GroundBlast::TargetCancelled);
	WaitTargetDataTask->ReadyForActivation();

	// 声明目标选择器Actor指针（用于接收生成的Actor）
	AGameplayAbilityTargetActor* TargetActor;
	// 始生成目标选择器Actor：创建Actor实例但不完成初始化
	WaitTargetDataTask->BeginSpawningActor(this,TargetActorClass,TargetActor);

	ATargetActor_GroundPick* GroundPickActor = Cast<ATargetActor_GroundPick>(TargetActor);
	if (GroundPickActor)
	{
		GroundPickActor->SetShouldDrawDebug(ShouldDrawDebug());
		GroundPickActor->SetTargetAreaRadius(TargetAreaRadius);
		GroundPickActor->SetTargetTraceRange(TargetTraceRange);
	}
	
	// 完成生成目标选择器Actor：完成Actor的初始化和注册
	WaitTargetDataTask->FinishSpawningActor(this,TargetActor);
}

void UGA_GroundBlast::TargetConfirmed(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	if (!K2_CommitAbility())
	{
		K2_EndAbility();
		return;
	}
	ACPlayerCharacter* Character = Cast<ACPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (!Character) // 检查指针是否为空
	{
		K2_EndAbility();
		return;
	}

	
	if (K2_HasAuthority()) 
	{
		// 对目标应用伤害效果
		// BP_ApplyGameplayEffectToTarget：蓝图实现的伤害应用函数
		// TargetDataHandle：目标数据容器，包含所有被选中的目标
		// DamageEffectDef.DamageEffect：配置的伤害效果类（GameplayEffect）
		// GetAbilityLevel()：获取当前技能等级，用于效果缩放
		BP_ApplyGameplayEffectToTarget(
			TargetDataHandle, 
			DamageEffectDef.DamageEffect, 
			GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo)
		);

		//  对目标施加推力效果（击退/击飞）
		// PushTargets：自定义函数，对目标施加物理推力
		// DamageEffectDef.PushVelocity：配置的推力向量（方向+大小）
		PushTargets(TargetDataHandle, DamageEffectDef.PushVelocity);
	}
	

	//  准备爆炸视觉效果参数
	// FGameplayCueParameters：游戏提示参数结构，用于传递视觉效果数据
	FGameplayCueParameters BlastingGameplayCueParameters;
    
	// 设置爆炸位置：从目标数据中提取命中点的位置
	// GetHitResultFromTargetData：从目标数据中获取命中结果
	// ImpactPoint：射线命中的具体位置（爆炸中心点）
	BlastingGameplayCueParameters.Location = 
		UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TargetDataHandle, 1).ImpactPoint;
    
	// 设置爆炸范围：用于视觉效果缩放（如爆炸半径影响特效大小）
	BlastingGameplayCueParameters.RawMagnitude = TargetAreaRadius;

	// 执行爆炸视觉效果（GameplayCue）
	// ExecuteGameplayCue：触发指定的游戏提示（视觉/音效效果）
	// BlastGameplayCueTag：爆炸特效的标签标识（如粒子、音效、抖动等）
	GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(
		BlastGameplayCueTag, 
		BlastingGameplayCueParameters
	);

	//  执行相机抖动效果
	// GetCameraShakeGameplayCueTag()：获取相机抖动效果的标签
	// 让相机根据爆炸强度产生抖动，增强冲击感
	GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(
		UCAbilitySystemStatics::GetCameraShakeGameplayCueTag(), 
		BlastingGameplayCueParameters
	);

	UAnimInstance* OwnerAnimInstance = GetOwnerAnimInstance();
	if (OwnerAnimInstance)
	{
		OwnerAnimInstance->Montage_Play(CastMontage);
	}

	// 结束技能执行
	// K2_EndAbility：结束当前技能的激活状态
	// 清理资源、进入冷却、恢复状态等
	K2_EndAbility();
}

void UGA_GroundBlast::TargetCancelled(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	UE_LOG(LogTemp, Warning, TEXT("Target Cancelled"));
	K2_EndAbility();
}
