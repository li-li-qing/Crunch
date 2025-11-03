// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/AbilityListView.h"
#include "Abilities/GameplayAbility.h"
#include "Widgets/AbilityGauge.h"

void UAbilityListView::ConfigureAbilities(const TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>>& Abilities)
{
	// 绑定列表项生成事件
	OnEntryWidgetGenerated().AddUObject(this,&UAbilityListView::AbilityGaugeGenerated);
	// 遍历所有技能配置
	for (const TPair<ECAbilityInputID,TSubclassOf<UGameplayAbility>>& AbilityPair : Abilities)
	{
		// 获取技能类的默认对象（CDO）并添加到列表
		AddItem(AbilityPair.Value.GetDefaultObject());
	}
}

void UAbilityListView::AbilityGaugeGenerated(UUserWidget& Widget)
{
	// 类型安全转换
	UAbilityGauge* AbilityGauge = Cast<UAbilityGauge>(&Widget);
	if (AbilityGauge)
	{
		// 获取列表项关联的技能对象
		UGameplayAbility* Ability = AbilityGauge->GetListItem<UGameplayAbility>();
        
		// 查找对应的UI配置数据
		const FAbilityWidgetData* WidgetData = FindWidgetDataForAbility(Ability->GetClass());
        
		// 配置技能图标显示
		AbilityGauge->ConfigureWidgetData(WidgetData);
	}
}

const FAbilityWidgetData* UAbilityListView::FindWidgetDataForAbility(
	const TSubclassOf<UGameplayAbility>& AbilityClass) const
{
	//  DataTable有效性检查
	if (!AbilityDataTable) return nullptr;

	// 遍历DataTable所有行
	for (auto& AbilityWidgetDataPair : AbilityDataTable->GetRowMap())
	{
		// 获取行数据（通过行名查找）
		const FAbilityWidgetData* WidgetData = AbilityDataTable->FindRow<FAbilityWidgetData>(
			AbilityWidgetDataPair.Key, // 行名称
			"" // 调试上下文（空字符串）
		);
        
		// 匹配技能类
		if (WidgetData && WidgetData->AbilityClass == AbilityClass)
		{
			return WidgetData; // 找到匹配项
		}
	}
    
	return nullptr; // 未找到匹配项
}
