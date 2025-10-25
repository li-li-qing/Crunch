// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/CAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"

void UCAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// 注册 Health 属性的同步规则：
	// - DOREPLIFETIME_CONDITION_NOTIFY: 宏用于定义同步条件
	//   - UCAttributeSet: 当前类
	//   - Health: 要同步的属性
	//   - COND_None: 无条件同步（总是同步）
	//   - REPNOTIFY_Always: 总是触发 OnRep 回调（即使新值 == 旧值）
	DOREPLIFETIME_CONDITION_NOTIFY(UCAttributeSet,Health,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCAttributeSet,MaxHealth,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCAttributeSet,Mana,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCAttributeSet,MaxMana,COND_None,REPNOTIFY_Always);
}

void UCAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue,0.f,GetMaxHealth());
	}
	if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue,0.f,GetMaxMana());
	}
}

void UCAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	// 检查被修改的属性是否是Health
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		// 钳制Health值到[0, MaxHealth]范围内
		SetHealth(FMath::Clamp(GetHealth(),0.f,GetMaxHealth()));
	}
	if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(),0.f,GetMaxMana()));
	}
}

void UCAttributeSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	// GAMEPLAYATTRIBUTE_REPNOTIFY 宏的作用：
	// 1. 触发 GAS 内部的属性更新回调（如 PreAttributeChange）
	// 2. 广播属性变化事件（用于 UI 或其他系统监听）
	// 参数：
	//   - UAttributeSet: 父类
	//   - Health: 当前属性
	//   - OldValue: 旧值
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCAttributeSet,Health,OldValue);
}

void UCAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCAttributeSet,MaxHealth,OldValue);
}

void UCAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCAttributeSet,Mana,OldValue);
}

void UCAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCAttributeSet,MaxMana,OldValue);
}
