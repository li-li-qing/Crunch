// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/AN_SendTargetGroup.h"
#include "AbilitySystemBlueprintLibrary.h"

void UAN_SendTargetGroup::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp) return;
	
	if (TargetSocketNames.Num() <= 1) return;

	if (!MeshComp->GetOwner() || !UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(MeshComp->GetOwner()))return;

	FGameplayEventData Data;

	for (int i=1;i<TargetSocketNames.Num();++i)
	{
		// 创建位置信息容器（用于GAS系统）
		FGameplayAbilityTargetData_LocationInfo * LocationInfo = new FGameplayAbilityTargetData_LocationInfo();
		// 获取线段起点和终点（骨骼插槽的世界坐标）
		FVector StartLoc = MeshComp->GetSocketLocation(TargetSocketNames[i-1]);
		FVector EndLoc = MeshComp->GetSocketLocation(TargetSocketNames[i]);
		// 设置起点位置
		LocationInfo->SourceLocation.LiteralTransform.SetLocation(StartLoc);
		// 设置终点位置
		LocationInfo->TargetLocation.LiteralTransform.SetLocation(EndLoc);
		// 将位置数据添加到事件负载中
		Data.TargetData.Add(LocationInfo);
	}
	//发送GameplayEvent到Owner的ASC
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(),EventTag,Data);
}
