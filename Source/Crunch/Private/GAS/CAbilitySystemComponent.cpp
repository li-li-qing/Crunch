// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/CAbilitySystemComponent.h"
#include "GAS/CAttributeSet.h"
#include "GAS/CGameplayAbilityTypes.h"
#include "GAS/CHeroAttributeSet.h"
UCAbilitySystemComponent::UCAbilitySystemComponent()
{
	// 绑定血量变更事件
	GetGameplayAttributeValueChangeDelegate(UCAttributeSet::GetHealthAttribute()).AddUObject(this,&UCAbilitySystemComponent::HealthUpdated);

	GenericConfirmInputID = (int32)ECAbilityInputID::Confirm;
	GenericCancelInputID = (int32)ECAbilityInputID::Cancel;

}

void UCAbilitySystemComponent::InitializeBaseAttributes()
{
	// 安全检查：确保数据表和所有者存在
	if (!BaseStatDataTable || !GetOwner()) return;
	// 声明基础属性指针，用于存储找到的匹配数据
	const FHeroBaseStats* BaseStats = nullptr;

	// 遍历DataTable中的所有行，寻找匹配的角色类配置
	// GetRowMap()：获取数据表的所有行映射（键值对集合）
	// TPair<FName, uint8*>：键=行名，值=行数据指针
	for (const TPair<FName,uint8*>& DataPair : BaseStatDataTable->GetRowMap())
	{
		// 查找当前行的数据：将原始数据转换为FHeroBaseStats结构
		// FindRow<FHeroBaseStats>：模板函数，查找指定类型的行数据
		// DataPair.Key：行名称（如"Warrior", "Mage"等）
		// ""：调试上下文（空字符串）
		BaseStats = BaseStatDataTable->FindRow<FHeroBaseStats>(DataPair.Key,"");
		// 检查是否找到匹配的角色类配置
		// BaseStats->Class：数据表中配置的角色类
		// GetOwner()->GetClass()：当前组件的所有者角色类
		if (BaseStats && BaseStats->Class == GetOwner()->GetClass())
		{
			break;
		}
	}
	// 如果找到匹配的基础属性配置
	if (BaseStats)
	{
		// SetNumericAttributeBase：设置属性的基础值（不受修饰器影响）
		SetNumericAttributeBase(UCAttributeSet::GetMaxHealthAttribute(),BaseStats->BaseMaxHealth);
		SetNumericAttributeBase(UCAttributeSet::GetMaxManaAttribute(),BaseStats->BaseMaxMana);
		SetNumericAttributeBase(UCAttributeSet::GetAttackDamageAttribute(),BaseStats->BaseAttackDamage);
		SetNumericAttributeBase(UCAttributeSet::GetArmorAttribute(),BaseStats->BaseArmor);
		SetNumericAttributeBase(UCAttributeSet::GetMoveSpeedAttribute(),BaseStats->BaseMoveSpeed);

		SetNumericAttributeBase(UCHeroAttributeSet::GetStrengthAttribute(),BaseStats->Strength);
		SetNumericAttributeBase(UCHeroAttributeSet::GetStrengthGrowthRateAttribute(),BaseStats->StrengthGrowthRate);
		
		SetNumericAttributeBase(UCHeroAttributeSet::GetIntelligenceAttribute(),BaseStats->Intelligence);
		SetNumericAttributeBase(UCHeroAttributeSet::GetIntelligenceGrowRateAttribute(),BaseStats->IntelligenceGrowthRate);

	}
	
}

void UCAbilitySystemComponent::ServerSideInit()
{
	InitializeBaseAttributes();
	ApplyInitialEffects();
	GiveInitialAbilities();
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

void UCAbilitySystemComponent::ApplyFullStatEffect()
{
	AuthApplyGameplayEffect(FullStatEffect);
}

const TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>>& UCAbilitySystemComponent::GetAbilities() const
{
	return Abilities;
}

void UCAbilitySystemComponent::HealthUpdated(const FOnAttributeChangeData& ChangeData)
{
	if (!GetOwner()) return;

	// 检查死亡条件：血量<=0、服务端权限、死亡效果存在
	if (ChangeData.NewValue <=0 && GetOwner()->HasAuthority() && DeathEffect)
	{
		AuthApplyGameplayEffect(DeathEffect);
	}
}

void UCAbilitySystemComponent::AuthApplyGameplayEffect(TSubclassOf<UGameplayEffect> GameplayEffect, int Level)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		// 创建效果句柄
		FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingSpec(GameplayEffect,Level,MakeEffectContext());
		// 把这个效果传给自己
		ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
	}
	
}


