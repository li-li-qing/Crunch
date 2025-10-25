// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/CAbilitySystemComponent.h"
#include "GAS/CAttributeSet.h"
UCAbilitySystemComponent::UCAbilitySystemComponent()
{
	// 绑定血量变更事件
	GetGameplayAttributeValueChangeDelegate(UCAttributeSet::GetHealthAttribute()).AddUObject(this,&UCAbilitySystemComponent::HealthUpdated);
}

void UCAbilitySystemComponent::ApplyInitialEffects()
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;

	// 遍历Effect数组中的所有数据
	for (const TSubclassOf<UGameplayEffect>& EffectClass : InitialEffect)
	{
		// 检查 EffectClass 是否有效，避免空指针崩溃
		if (!EffectClass)
		{
			UE_LOG(LogTemp, Warning, TEXT("Invalid EffectClass in InitialEffect array!"));
			continue;
		}
		// 创建 GameplayEffect 的规格（Spec）句柄
		// EffectClass: 要应用的 GameplayEffect 类
		// Level: 效果等级（这里传 1，可根据需求调整）
		// Context: 效果上下文（包含施法者、目标等信息）
		FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingSpec(EffectClass,1,MakeEffectContext());
		// 检查 Spec 是否创建成功
		if (!EffectSpecHandle.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to create EffectSpec for %s!"), *EffectClass->GetName());
			continue;
		}
		// 将 GameplayEffect 应用到自身
		// *EffectSpecHandle.Data.Get(): 获取 Spec 的数据指针
		ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
	}
}

void UCAbilitySystemComponent::GiveInitialAbilities()
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;

	// 遍历 Abilities 数组中的所有数据
	for (const TPair<ECAbilityInputID, TSubclassOf<UGameplayAbility>>& AbilityPair : Abilities)
	{
		// 创建技能规格（AbilitySpec）并赋予角色
		// 参数说明：
		// - AbilityClass：技能类
		// - 0：技能等级（0表示未学习）
		// - -1：输入绑定ID（-1表示不绑定按键）
		// - nullptr：来源对象（通常为null）
		GiveAbility(FGameplayAbilitySpec(AbilityPair.Value,0,(int32)AbilityPair.Key,nullptr));
	}
	// 遍历 BasicAbilities 数组中的所有数据
	for (const TPair<ECAbilityInputID, TSubclassOf<UGameplayAbility>>&  AbilityPair: BasicAbilities)
	{
		// InLevel等级为1,则为学习了这个技能
		GiveAbility(FGameplayAbilitySpec(AbilityPair.Value,0,(int32)AbilityPair.Key,nullptr));
	}
	
}

void UCAbilitySystemComponent::HealthUpdated(const FOnAttributeChangeData& ChangeData)
{
	if (!GetOwner()) return;

	// 检查死亡条件：血量<=0、服务端权限、死亡效果存在
	if (ChangeData.NewValue <=0 && GetOwner()->HasAuthority() && DeathEffect)
	{
		// 创建死亡效果实例
		FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingSpec(DeathEffect,1,MakeEffectContext());
		// 应用效果到自身
		ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
	}
}


