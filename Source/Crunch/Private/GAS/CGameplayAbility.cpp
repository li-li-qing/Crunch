// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/CGameplayAbility.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Character.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "CAbilitySystemComponent.h"
#include "CAbilitySystemStatics.h"
#include "GAS/GAP_Launch.h"

UCGameplayAbility::UCGameplayAbility()
{
	// 对阻挡增加 眩晕 标签
	ActivationBlockedTags.AddTag(UCAbilitySystemStatics::GetStunStatTag());
}

UAnimInstance* UCGameplayAbility::GetOwnerAnimInstance() const
{
	USkeletalMeshComponent* OwnerSkeletalMeshComponent = GetOwningComponentFromActorInfo();
	if (OwnerSkeletalMeshComponent)
	{
		return OwnerSkeletalMeshComponent->GetAnimInstance();
	}
	return nullptr;
}

TArray<FHitResult> UCGameplayAbility::GetHitResultFromSweepLocationTargetData(
	const FGameplayAbilityTargetDataHandle& TargetDataHandle, float SphereSweepRadius, ETeamAttitude::Type TargetTeam,
	bool bDrawDebug, bool bIgnoreSelf) const
{
	TArray<FHitResult> OutResults;
	TSet<AActor*> HitActors;
	// 从施法者身上获取团队接口
	IGenericTeamAgentInterface* OwnerTeamAgentInterface = Cast<IGenericTeamAgentInterface>(
		GetAvatarActorFromActorInfo());
	// 1. 遍历目标数据容器中的所有数据项
	for (const TSharedPtr<FGameplayAbilityTargetData> TargetData : TargetDataHandle.Data)
	{
		// 2. 获取扫描的起点和终点坐标
		FVector StartLoc = TargetData->GetOrigin().GetTranslation(); // 起点位置
		FVector EndLoc = TargetData->GetEndPoint(); // 终点位置

		// 3. 设置碰撞对象类型（只检测Pawn类型）
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn)); // 仅检测Pawn

		// 4. 设置忽略的Actor列表
		TArray<AActor*> ActorsToIgnore;
		if (bIgnoreSelf)
		{
			// 忽略技能施法者自身（避免自伤）
			ActorsToIgnore.Add(GetAvatarActorFromActorInfo());
		}

		// 5. 设置调试可视化选项
		EDrawDebugTrace::Type DrawDebugTrace =
			bDrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

		// 6. 执行球体扫描检测
		TArray<FHitResult> Results;
		UKismetSystemLibrary::SphereTraceMultiForObjects(
			this, // [IN] WorldContext对象（当前技能）
			StartLoc, // [IN] 扫描起点
			EndLoc, // [IN] 扫描终点
			SphereSweepRadius, // [IN] 球体半径
			ObjectTypes, // [IN] 检测的对象类型
			false, // [IN] 是否使用复杂碰撞（false=简单碰撞）
			ActorsToIgnore, // [IN] 忽略的Actor列表
			DrawDebugTrace, // [IN] 调试绘制选项
			Results, // [OUT] 命中结果数组
			false // [IN] 是否忽略阻挡（false=检测阻挡）
		);
		for (const FHitResult& Result : Results)
		{
			if (HitActors.Contains(Result.GetActor()))
			{
				continue;
			}
			if (OwnerTeamAgentInterface)
			{
				// 获取当前角色对另一个Actor的团队态度	
				ETeamAttitude::Type OtherActorTeamAttitude = OwnerTeamAgentInterface->GetTeamAttitudeTowards(
					*Result.GetActor());
				if (OtherActorTeamAttitude != TargetTeam)
				{
					continue;
				}
			}
			HitActors.Add(Result.GetActor());
			OutResults.Add(Result);
		}
	}
	// 返回所有扫描结果
	return OutResults;
}

void UCGameplayAbility::PushSelf(const FVector& PushVel)
{
	// 获取技能施法者的角色对象
	ACharacter* OwningAvatarCharacter = GetOwningAvatarCharacter();
	if (OwningAvatarCharacter)
	{
		// 对角色施加推力
		OwningAvatarCharacter->LaunchCharacter(PushVel, true, true);
	}
}

void UCGameplayAbility::PushTarget(AActor* Target, const FVector& PushVel)
{
	// ► 1. 安全检查：目标是否有效
	if (!Target) return;

	// ► 2. 创建Gameplay事件数据容器
	FGameplayEventData EventData;

	// ► 3. 创建单目标碰撞数据
	FGameplayAbilityTargetData_SingleTargetHit* HitData = new FGameplayAbilityTargetData_SingleTargetHit();

	// ► 4. 设置碰撞结果（用于传递推力方向）
	FHitResult HitResult;
	HitResult.ImpactNormal = PushVel; // 使用推力向量作为碰撞法线
	HitData->HitResult = HitResult; // 将碰撞结果存入目标数据

	// ► 5. 将目标数据添加到事件数据
	EventData.TargetData.Add(HitData);

	// ► 6. 向目标Actor发送起飞事件
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		Target, // 接收事件的目标
		UGAP_Launch::GetLauchedAbilityActivationTag(), // 起飞事件标签
		EventData // 包含推力数据的事件
	);
}

ACharacter* UCGameplayAbility::GetOwningAvatarCharacter()
{
	if (!AvatarCharacter)
	{
		AvatarCharacter = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	}
	return AvatarCharacter;
}

void UCGameplayAbility::ApplyGameplayEffectToHitResultActor(const FHitResult& HitResult,
                                                            TSubclassOf<UGameplayEffect> GameplayEffect, int Level)
{
	// 创建GameplayEffectSpec（效果规格）
	FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(GameplayEffect, Level);

	// 创建并配置GameplayEffect的上下文（Context）
	FGameplayEffectContextHandle EffectContext = MakeEffectContext(GetCurrentAbilitySpecHandle(),
	                                                               GetCurrentActorInfo());
	// 将命中结果（HitResult）添加到上下文中
	EffectContext.AddHitResult(HitResult);
	// 将上下文绑定到GameplayEffectSpec
	EffectSpecHandle.Data->SetContext(EffectContext);

	// 将效果应用到命中的目标
	ApplyGameplayEffectSpecToTarget(GetCurrentAbilitySpecHandle(), CurrentActorInfo, CurrentActivationInfo,
	                                EffectSpecHandle,
	                                UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(
		                                HitResult.GetActor()));
}
