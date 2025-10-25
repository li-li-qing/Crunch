// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OverHeadStatsGauge.generated.h"

class UAbilitySystemComponent;
class UValueGauge;
/**
 * @brief 玩家头顶的血条
 */
UCLASS()
class UOverHeadStatsGauge : public UUserWidget
{
	GENERATED_BODY()
public:
	void ConfigureWithASC( UAbilitySystemComponent* AbilitySystemComponent);
private:
	// 血条
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UValueGauge> HealthBar;
	// 蓝条
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UValueGauge> ManaBar;
};
