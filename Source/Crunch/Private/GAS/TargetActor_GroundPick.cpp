// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/TargetActor_GroundPick.h"
#include "Crunch/Crunch.h"
#include "Engine/OverlapResult.h"
#include "GenericTeamAgentInterface.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Components/DecalComponent.h"
ATargetActor_GroundPick::ATargetActor_GroundPick()
{
	PrimaryActorTick.bCanEverTick = true;
	SetRootComponent(CreateDefaultSubobject<USceneComponent>("Root"));
	DecalComp = CreateDefaultSubobject<UDecalComponent>(TEXT("Decal Comp"));
	DecalComp->SetupAttachment(GetRootComponent());
}

void ATargetActor_GroundPick::ConfirmTargetingAndContinue()
{
	 // 执行球形重叠检测，收集区域内的所有Pawn
    TArray<FOverlapResult> OverlapResults;

    // 配置碰撞对象查询参数：只检测Pawn类型的物体
    FCollisionObjectQueryParams ObjectQueryParams;
    ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn); // 设置碰撞通道为Pawn

    // 配置碰撞形状：球形检测区域
    FCollisionShape CollisionShape;
    CollisionShape.SetSphere(TargetAreaRadius); // 设置球形检测半径

    // 执行球形重叠检测
    GetWorld()->OverlapMultiByObjectType(
        OverlapResults,         // [OUT] 重叠检测结果数组
        GetActorLocation(),     // [IN] 检测中心点（目标选择器当前位置）
        FQuat::Identity,        // [IN] 旋转（无旋转）
        ObjectQueryParams,      // [IN] 碰撞对象查询参数
        CollisionShape          // [IN] 碰撞形状（球形）
    );

    // 准备存储有效目标的集合
    TSet<AActor*> TargetActors;
    
    //  获取技能施放者的团队接口（用于团队关系判断）
    IGenericTeamAgentInterface* OwnerTeamAgentInterface = nullptr;
    if (OwningAbility)
    {
        // 从技能获取施法者AvatarActor并转换为团队接口
        OwnerTeamAgentInterface = Cast<IGenericTeamAgentInterface>(OwningAbility->GetAvatarActorFromActorInfo());
    }
    
    //  遍历所有重叠检测结果，过滤有效目标
    for (const FOverlapResult& OverlapResult : OverlapResults)
    {
        AActor* OverlapActor = OverlapResult.GetActor();
        if (!OverlapActor) continue;

        // 团队关系过滤：检查是否应该目标友方单位
        if (OwnerTeamAgentInterface && 
            OwnerTeamAgentInterface->GetTeamAttitudeTowards(*OverlapActor) == ETeamAttitude::Friendly && 
            !bShouldTargetFriendly)
        {
            continue; // 跳过友方单位
        }

        // 团队关系过滤：检查是否应该目标敌方单位
        if (OwnerTeamAgentInterface && 
            OwnerTeamAgentInterface->GetTeamAttitudeTowards(*OverlapActor) == ETeamAttitude::Hostile && 
            !bShouldTargetEnemy)
        {
            continue; // 跳过敌方单位
        }
        
        // 通过所有过滤条件，添加到有效目标集合
        TargetActors.Add(OverlapActor);
    }

    //  将目标Actor集合转换为GameplayAbility目标数据
    FGameplayAbilityTargetDataHandle TargetData = 
        UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActorArray(
            TargetActors.Array(),  // 目标Actor数组
            false                 // 是否包含源Actor
        );

	// 为爆炸特效做一一个位置信息的准备
	FGameplayAbilityTargetData_SingleTargetHit* HitLoc = new FGameplayAbilityTargetData_SingleTargetHit;
	HitLoc->HitResult.ImpactPoint = GetActorLocation();

	TargetData.Add(HitLoc);
	
    //  广播目标数据就绪事件，通知技能系统继续执行
    TargetDataReadyDelegate.Broadcast(TargetData);
}

void ATargetActor_GroundPick::SetTargetAreaRadius(float NewRadius)
{
	TargetAreaRadius = NewRadius;
	DecalComp->DecalSize = FVector{NewRadius};
}

void ATargetActor_GroundPick::SetTargetOptions(bool bTargetFriendly, bool bTargetEnemy)
{
	bShouldTargetFriendly = bTargetFriendly;
	bShouldTargetEnemy = bTargetEnemy;
}

void ATargetActor_GroundPick::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// 检查是否是本地玩家控制器（避免在服务端或其他客户端更新）
	if (PrimaryPC && PrimaryPC->IsLocalPlayerController())
	{
		// 更新目标选择器位置到计算出的目标点
		SetActorLocation(GetTargetPoint());
	}
}

FVector ATargetActor_GroundPick::GetTargetPoint() const
{
	// 安全检查：确保有有效的本地玩家控制器
	if (!PrimaryPC || !PrimaryPC->IsLocalPlayerController()) return GetActorLocation();

	FHitResult TraceResult; // 射线检测结果

	// 获取玩家摄像机视角信息
	FVector ViewLoc; // 摄像机位置
	FRotator ViewRot; // 摄像机旋转

	// 计算射线终点：从摄像机位置沿视角方向延伸一定距离
	PrimaryPC->GetPlayerViewPoint(ViewLoc, ViewRot);

	FVector TraceEnd = ViewLoc + ViewRot.Vector() * TargetTraceRange;

	// 第一次射线检测：从摄像机向视角方向检测
	GetWorld()->LineTraceSingleByChannel(
		TraceResult, // [OUT] 命中结果
		ViewLoc, // [IN] 射线起点（摄像机位置）
		TraceEnd, // [IN] 射线终点
		ECC_Target // [IN] 碰撞通道（可见物体）
	);

	// 如果第一次检测未命中，进行第二次检测：从射线终点垂直向下检测地面
	if (!TraceResult.bBlockingHit)
	{
		GetWorld()->LineTraceSingleByChannel(
			TraceResult,
			TraceEnd, // 起点：第一次射线的终点
			TraceEnd + FVector::DownVector * TNumericLimits<float>::Max(), // 终点：垂直向下无限远
			ECC_Target
		);
	}

	// 如果两次检测都未命中，返回当前位置
	if (!TraceResult.bBlockingHit)
	{
		return GetActorLocation();
	}

	if (bShouldDrawDebug)
	{
		DrawDebugSphere(GetWorld(), TraceResult.ImpactPoint, TargetAreaRadius, 32, FColor::Red);
	}
	
	// 返回射线命中的碰撞点
	return TraceResult.ImpactPoint;
}
