// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityGauge.h"
#include "Components/ListView.h"
#include "GAS/CGameplayAbilityTypes.h"
#include "AbilityListView.generated.h"

struct FAbliyWidgetData;
/**
 * 技能列表视图类（管理多个技能图标）
 */
UCLASS()
class UAbilityListView : public UListView
{
	GENERATED_BODY()

public:
	// 核心配置函数：将输入ID映射的技能类配置到列表
	void ConfigureAbilities(const TMap<ECAbilityInputID, TSubclassOf<class UGameplayAbility>>& Abilities);


private:
	// DataTable资源引用：存储技能UI配置数据
	UPROPERTY(EditAnywhere,Category = "Data")
	UDataTable* AbilityDataTable;

	// 列表项生成回调：每个技能图标创建时调用
	void AbilityGaugeGenerated(UUserWidget& Widget);

	// 数据查询：根据技能类查找对应的UI配置
	const FAbilityWidgetData* FindWidgetDataForAbility(const TSubclassOf<UGameplayAbility>& AbilityClass) const;
};
