// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GAS/CGameplayAbilityTypes.h"
#include "GameplayWidget.generated.h"


class UAbilitySystemComponent;
class UValueGauge;
class UAbilityListView;
/**
 * @brief 主UI界面
 */
UCLASS()
class UGameplayWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	// 配置技能
	void ConfigureAbilities(const TMap<ECAbilityInputID,TSubclassOf<class UGameplayAbility>>& Abilities);
	
private:
	// 血条
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UValueGauge> HealthBar;
	// 蓝条
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UValueGauge> ManaBar;

	// 技能列表
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UAbilityListView> AbilityListView;
	
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> OwnerAbilitySystemComponent;
};
