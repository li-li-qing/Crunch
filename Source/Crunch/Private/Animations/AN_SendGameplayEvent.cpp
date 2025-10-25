// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/AN_SendGameplayEvent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTagsManager.h"

void UAN_SendGameplayEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	if (!MeshComp->GetOwner()) return;
	UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(MeshComp->GetOwner());
	if (!OwnerASC) return;
	// 向Owner发送GameplayEvent事件
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(),EventTag,FGameplayEventData());
}

FString UAN_SendGameplayEvent::GetNotifyName_Implementation() const
{

	if (EventTag.IsValid())
	{
		// 分割GameplayTag为层级结构（如"Event.Attack.Hit" -> ["Event", "Attack", "Hit"]）
		TArray<FName> TagNames;
		UGameplayTagsManager::Get().SplitGameplayTagFName(EventTag, TagNames);
		// 取最后一级作为显示名称（如"Hit"）
		return TagNames.Last().ToString();
	}
	// 默认返回"None"
	return "None";
}
