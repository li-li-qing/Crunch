// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/GameplayWidget.h"
#include"AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "ValueGauge.h"
#include "GAS/CAttributeSet.h"

void UGameplayWidget::NativeConstruct()
{
	Super::NativeConstruct();
	// 获取玩家角色的AbilitySystemComponent
	OwnerAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerPawn());
	if (OwnerAbilitySystemComponent)
	{
		// 绑定血条到Health/MaxHealth属性
		HealthBar->SetAndBoundToGameplayAttribute(OwnerAbilitySystemComponent, UCAttributeSet::GetHealthAttribute(),
		                                          UCAttributeSet::GetMaxHealthAttribute());
		// 绑定蓝条到Mana/MaxMana属性
		ManaBar->SetAndBoundToGameplayAttribute(OwnerAbilitySystemComponent, UCAttributeSet::GetManaAttribute(),
		                                        UCAttributeSet::GetMaxManaAttribute());
	}
}
