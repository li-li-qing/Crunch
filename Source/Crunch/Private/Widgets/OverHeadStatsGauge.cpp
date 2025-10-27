// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/OverHeadStatsGauge.h"
#include"AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "ValueGauge.h"
#include "GAS/CAttributeSet.h"

void UOverHeadStatsGauge::ConfigureWithASC(UAbilitySystemComponent* AbilitySystemComponent)
{
	
	if (AbilitySystemComponent)
	{
		// 绑定血条到Health/MaxHealth属性
		HealthBar->SetAndBoundToGameplayAttribute(AbilitySystemComponent, UCAttributeSet::GetHealthAttribute(),
												  UCAttributeSet::GetMaxHealthAttribute());
		// 绑定蓝条到Mana/MaxMana属性
		ManaBar->SetAndBoundToGameplayAttribute(AbilitySystemComponent, UCAttributeSet::GetManaAttribute(),
												UCAttributeSet::GetMaxManaAttribute());
	}
}
