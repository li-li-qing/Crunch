// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CAIController.generated.h"
class UAISenseConfig_Sight;
class UAIPerceptionComponent;
/**
 * AI控制器
 */
UCLASS()
class ACAIController : public AAIController
{
	GENERATED_BODY()
public:
	ACAIController();
	virtual void OnPossess(APawn* Possessed)override;
private:
	// AI感知组件
	UPROPERTY(VisibleDefaultsOnly,Category="Perception")
	TObjectPtr<UAIPerceptionComponent> AIPerceptionComponent;
	// 视觉感知配置
	UPROPERTY(VisibleDefaultsOnly,Category="Perception")
	TObjectPtr<UAISenseConfig_Sight> SightConfig;
};
