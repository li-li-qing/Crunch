// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/AN_SendTargetGroup.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemGlobals.h"
#include "GameplayCueManager.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameplayEffectTypes.h"
#include "Abilities/GameplayAbilityTargetTypes.h"

void UAN_SendTargetGroup::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	// 安全检查：确保骨骼网格组件存在
	if (!MeshComp) return;
	
	// 安全检查：至少需要2个插槽名称才能形成线段检测
	if (TargetSocketNames.Num() <= 1) return;

	// 安全检查：确保拥有者存在且拥有能力系统组件
	if (!MeshComp->GetOwner() || !UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(MeshComp->GetOwner())) return;

	// 初始化游戏事件数据
	FGameplayEventData Data;
	// 用于记录已命中的Actor，避免重复命中
	TSet<AActor*> HitActors;
	
	// 获取拥有者Actor及其团队接口
	AActor* OwnerActor = MeshComp->GetOwner();
	const IGenericTeamAgentInterface* OwnerTeamInterface = Cast<IGenericTeamAgentInterface>(OwnerActor);
	
	// 遍历所有插槽，每两个相邻插槽之间进行检测
	for (int i = 1; i < TargetSocketNames.Num(); ++i)
	{
		// 获取线段起点和终点（骨骼插槽的世界坐标）
		FVector StartLoc = MeshComp->GetSocketLocation(TargetSocketNames[i-1]);
		FVector EndLoc = MeshComp->GetSocketLocation(TargetSocketNames[i]);

		// 设置检测对象类型：只检测Pawn
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
		
		// 存储命中结果
		TArray<FHitResult> HitResults;

		// 设置忽略的Actor列表
		TArray<AActor*> ActorsToIgnore;
		if (bIgnoreOwner)
		{
			// 如果启用了忽略拥有者，则将拥有者加入忽略列表
			ActorsToIgnore.Add(OwnerActor);
		}
		
		// 根据调试标志设置调试绘制类型
		EDrawDebugTrace::Type DrawDebugTrace = bDrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;
		
		// 执行球体扫描检测
		UKismetSystemLibrary::SphereTraceMultiForObjects(
			MeshComp,               // 世界上下文对象
			StartLoc,              // 起点位置
			EndLoc,                // 终点位置
			SphereSweepRadius,     // 球体半径
			ObjectTypes,           // 检测对象类型
			false,                 // 是否复杂碰撞
			ActorsToIgnore,        // 忽略的Actor列表
			DrawDebugTrace,        // 调试绘制设置
			HitResults,            // 命中结果输出
			false                  // 是否忽略自身
		);

		// 处理所有命中结果
		for (const FHitResult& HitResult : HitResults)
		{
			// 跳过已命中的Actor
			if (HitActors.Contains(HitResult.GetActor()))
			{
				continue;
			}

			// 团队关系检查
			if (OwnerTeamInterface)
			{
				// 如果命中Actor的团队态度不符合目标团队要求，则跳过
				if (OwnerTeamInterface->GetTeamAttitudeTowards(*HitResult.GetActor()) != TargetTeam)
				{
					continue;
				}
			}
			
			// 创建目标数据并添加到事件数据中
			FGameplayAbilityTargetData_SingleTargetHit* TargetHit = new FGameplayAbilityTargetData_SingleTargetHit(HitResult);
			Data.TargetData.Add(TargetHit);
			
			// 发送本地GameplayCue效果
			SendLocalGameplaCue(HitResult);
			
			// 记录已命中的Actor
			HitActors.Add(HitResult.GetActor());
		}
	}
	
	// 发送GameplayEvent到拥有者的能力系统组件
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(), EventTag, Data);
}

void UAN_SendTargetGroup::SendLocalGameplaCue(const FHitResult& HitResult) const
{
	// 设置GameplayCue参数
	FGameplayCueParameters CueParam;
	CueParam.Location = HitResult.Location;  // 命中位置
	CueParam.Normal = HitResult.Normal;      // 命中法线

	// 遍历所有需要触发的GameplayCue标签
	for (const FGameplayTag& GameplayCueTag : TriggerGameplayCueTags)
	{
		// 在命中的Actor上执行GameplayCue
		UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(
			HitResult.GetActor(),      // 目标Actor
			GameplayCueTag,            // GameplayCue标签
			EGameplayCueEvent::Executed, // 事件类型：执行
			CueParam                   // 参数
		);
	}
}
