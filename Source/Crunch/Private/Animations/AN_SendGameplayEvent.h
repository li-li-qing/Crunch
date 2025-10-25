// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "AN_SendGameplayEvent.generated.h"

/**
 * 
 */
UCLASS()
class UAN_SendGameplayEvent : public UAnimNotify
{
	GENERATED_BODY()

public:
	/**
	 * @brief 在动画播放到指定帧时，向角色发送GameplayEvent事件
	 * @param MeshComp 触发通知的骨骼网格组件
	 * @param Animation 关联的动画资源
	 * @param EventReference 通知事件的上下文引用
	 */
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)override;

	virtual FString GetNotifyName_Implementation() const override;
	
	UPROPERTY(EditAnywhere,Category="Gameplay Ability")
	FGameplayTag EventTag;
};
