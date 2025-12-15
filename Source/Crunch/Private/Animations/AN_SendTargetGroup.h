// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "GenericTeamAgentInterface.h"
#include "AN_SendTargetGroup.generated.h"

/**
 * 
 */
UCLASS()
class UAN_SendTargetGroup : public UAnimNotify
{
	GENERATED_BODY()
public:
	/**
	 * @brief 发送多目标位置信息（用于攻击范围检测）
	 * @param MeshComp 
	 * @param Animation 
	 * @param EventReference 
	 */
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	UPROPERTY(EditAnywhere,Category="Gameplay Ability")
	FGameplayTagContainer TriggerGameplayCueTags;

	UPROPERTY(EditAnywhere,Category="Gameplay Ability")
	float SphereSweepRadius = 60.f;
	
	UPROPERTY(EditAnywhere,Category="Gameplay Ability")
	TEnumAsByte<ETeamAttitude::Type> TargetTeam = ETeamAttitude::Hostile;

	UPROPERTY(EditAnywhere,Category="Gameplay Ability")
	bool bDrawDebug = true;

	UPROPERTY(EditAnywhere,Category="Gameplay Ability")
	bool bIgnoreOwner = true;
	
	UPROPERTY(EditAnywhere,Category="Gameplay Ability")
	FGameplayTag EventTag;
	// 存放所有的插槽名字
	UPROPERTY(EditAnywhere,Category="Gameplay Ability")
	TArray<FName> TargetSocketNames;


	void SendLocalGameplaCue(const FHitResult& HitResult) const;
	
};
