// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "GAS/CGameplayAbilityTypes.h"
#include "BBTask_SendInputToAbilitySystem.generated.h"

/**
 * 
 */
UCLASS()
class UBBTask_SendInputToAbilitySystem : public UBTTaskNode
{
	GENERATED_BODY()
public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
private:
	UPROPERTY(EditAnywhere,Category = "Ability")
	ECAbilityInputID InputID;
};
