// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/CAbilitySystemStatics.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemInterface.h"

FGameplayTag UCAbilitySystemStatics::GetBasicAttackAbilityTag()
{
	return FGameplayTag::RequestGameplayTag("Ability.BasicAttack");
}

FGameplayTag UCAbilitySystemStatics::GetBasicAttackInputPressedTag()
{
	return FGameplayTag::RequestGameplayTag("Ability.BasicAttack.Pressed");
}

FGameplayTag UCAbilitySystemStatics::GetDeadStatTag()
{
	return FGameplayTag::RequestGameplayTag("Stats.Dead");
}

FGameplayTag UCAbilitySystemStatics::GetStunStatTag()
{
	return FGameplayTag::RequestGameplayTag("Stats.Stun");
}

float UCAbilitySystemStatics::GetStaticCooldownDurationForAbility(const UGameplayAbility* Ability)
{
	// 安全检查：验证技能指针有效性
	if (!Ability) return 0.0f;

	//  获取技能关联的冷却效果（GameplayEffect）
	const UGameplayEffect* CooldownEffect = Ability->GetCooldownGameplayEffect();

	//  检查冷却效果是否存在
	if (!CooldownEffect) return 0.0f;

	// 初始化冷却时间变量
	float CooldownDuration = 0.f;

	//  从效果配置中提取静态冷却时长
	CooldownEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(1, CooldownDuration);

	//  返回提取的冷却时间
	return CooldownDuration;
}

float UCAbilitySystemStatics::GetStaticCostForAbility(const UGameplayAbility* Ability)
{
	// 安全检查：验证技能指针有效性
	if (!Ability) return 0.0f;

	// 获取技能关联的消耗效果（GameplayEffect）
	const UGameplayEffect* CostEffect = Ability->GetCostGameplayEffect();
	if (!CostEffect || CostEffect->Modifiers.Num() == 0) return 0.0f;

	// 初始化消耗变量
	float Cost = 0.f;
	// 从第一个效果修饰器中提取静态消耗值
	CostEffect->Modifiers[0].ModifierMagnitude.GetStaticMagnitudeIfPossible(1, Cost);

	// 返回提取的资源消耗
	return FMath::Abs(Cost);
}

FGameplayTag UCAbilitySystemStatics::GetAimStatTag()
{
	return FGameplayTag::RequestGameplayTag("Stats.Aim");
}

FGameplayTag UCAbilitySystemStatics::GetCameraShakeGameplayCueTag()
{
	return FGameplayTag::RequestGameplayTag("GameplayCue.CameraShake");
}

FGameplayTag UCAbilitySystemStatics::GetHealthFullStatTag()
{
	return FGameplayTag::RequestGameplayTag("Stats.Health.Full");
}

FGameplayTag UCAbilitySystemStatics::GetHealthEmptyStatTag()
{
	return FGameplayTag::RequestGameplayTag("Stats.Health.Empty");
}

FGameplayTag UCAbilitySystemStatics::GetManaFullStatTag()
{
	return FGameplayTag::RequestGameplayTag("Stats.Mana.Full");
}

FGameplayTag UCAbilitySystemStatics::GetManaEmptyStatTag()
{
	return FGameplayTag::RequestGameplayTag("Stats.Mana.Empty");
}

FGameplayTag UCAbilitySystemStatics::GetHeroRoleTag()
{
	return FGameplayTag::RequestGameplayTag("Role.Hero");
}

FGameplayTag UCAbilitySystemStatics::GetExperienceAttributeTag()
{
	return FGameplayTag::RequestGameplayTag("Attribute.Experience");
}

FGameplayTag UCAbilitySystemStatics::GetGoldAttributeTag()
{
	return FGameplayTag::RequestGameplayTag("Attribute.Gold");
}

bool UCAbilitySystemStatics::IsHero(const AActor* ActorToCheck)
{
	// 将Actor转换为AbilitySystemInterface接口
	// IAbilitySystemInterface是GAS系统的核心接口，用于获取AbilitySystemComponent
	const IAbilitySystemInterface* ActorISA = Cast<IAbilitySystemInterface>(ActorToCheck);
	// 检查Actor是否实现了IAbilitySystemInterface接口
	if (ActorISA)
	{
		// 通过接口获取Actor的AbilitySystemComponent（GAS组件）
		// AbilitySystemComponent是GAS系统的核心组件，负责管理技能、属性和效果
		UAbilitySystemComponent* ActorASC = ActorISA->GetAbilitySystemComponent();
		      
		// 检查是否成功获取到AbilitySystemComponent
		if (ActorASC)
		{
			// 检查Actor是否拥有"Role.Hero"的GameplayTag
			// GetHeroRoleTag()返回标识英雄角色的标签（如"Role.Hero"）
			// HasMatchingGameplayTag()检查组件是否包含指定的标签
			return ActorASC->HasMatchingGameplayTag(GetHeroRoleTag());
		}
	}
	// 如果Actor没有实现接口或没有GAS组件，返回false（不是英雄）
	return false;
}

bool UCAbilitySystemStatics::IsAbilityAtMaxLevel(const FGameplayAbilitySpec& Spec)
{
	return Spec.Level >= 4;
}


bool UCAbilitySystemStatics::CheckAbilityCost(const FGameplayAbilitySpec& AbilitySpec,
											  const UAbilitySystemComponent& AbilitySystemComponent)
{
	// 从技能规格中获取技能的类默认对象（CDO）
	// AbilitySpec.Ability：指向技能实例的指针
	const UGameplayAbility* AbilityCDO = AbilitySpec.Ability;
    
	// 检查技能CDO是否有效
	if (AbilityCDO)
	{
		// 调用技能的CheckCost方法验证资源消耗
		// AbilitySpec.Handle：技能的唯一标识符
		// AbilitySystemComponent->AbilityActorInfo.Get()：获取技能拥有者的信息
		return AbilityCDO->CheckCost(AbilitySpec.Handle, AbilitySystemComponent.AbilityActorInfo.Get());
	}
    
	// 如果技能CDO无效，返回false（无法使用技能）
	return false;
}

float UCAbilitySystemStatics::GetManaCostFor(const UGameplayAbility* AbilityCDO, const UAbilitySystemComponent& ASC,
	int AbilityLevel)
{
	// 初始化魔法消耗值
	float ManaCost = 0.0f;
    
	// 检查技能CDO是否有效
	if (AbilityCDO)
	{
		// 从技能中获取消耗效果（GameplayEffect）
		// GetCostGameplayEffect()：返回技能配置的消耗效果
		UGameplayEffect* CostEffect = AbilityCDO->GetCostGameplayEffect();
        
		// 检查消耗效果是否存在
		if (CostEffect)
		{
			// 创建GameplayEffect规格（Spec）
			// MakeOutgoingSpec()：创建效果规格，包含效果配置和上下文
			// 参数1：GameplayEffect类
			// 参数2：效果等级（通常等于技能等级）
			// 参数3：效果上下文（包含施法者、目标等信息）
			FGameplayEffectSpecHandle EffectSpec = ASC.MakeOutgoingSpec(
				CostEffect->GetClass(), 
				AbilityLevel, 
				ASC.MakeEffectContext()
			);
            
			// 从效果规格中计算魔法消耗数值
			// CostEffect->Modifiers[0]：获取第一个修饰器（通常是魔法消耗）
			// AttemptCalculateMagnitude()：尝试计算修饰器的数值大小
			// 参数1：效果规格数据
			// 参数2：输出参数，接收计算出的数值
			CostEffect->Modifiers[0].ModifierMagnitude.AttemptCalculateMagnitude(
				*EffectSpec.Data.Get(), 
				ManaCost
			);
		}
	}
    
	// 返回消耗的绝对值（确保为正数）
	// FMath::Abs()：取绝对值，防止负值消耗
	return FMath::Abs(ManaCost);
	
}

float UCAbilitySystemStatics::GetCooldownDurationFor(const UGameplayAbility* AbilityCDO, const UAbilitySystemComponent& ASC,
	int AbilityLevel)
{

	float CooldownDuration = 0.0f;
    
	// 检查技能CDO是否有效
	if (AbilityCDO)
	{
		// 从技能中获取消耗效果（GameplayEffect）
		// GetCostGameplayEffect()：返回技能配置的消耗效果
		UGameplayEffect* CooldownEffect = AbilityCDO->GetCooldownGameplayEffect();
        
		// 检查消耗效果是否存在
		if (CooldownEffect)
		{
			// 创建GameplayEffect规格（Spec）
			// MakeOutgoingSpec()：创建效果规格，包含效果配置和上下文
			// 参数1：GameplayEffect类
			// 参数2：效果等级（通常等于技能等级）
			// 参数3：效果上下文（包含施法者、目标等信息）
			FGameplayEffectSpecHandle EffectSpec = ASC.MakeOutgoingSpec(
				CooldownEffect->GetClass(), 
				AbilityLevel, 
				ASC.MakeEffectContext()
			);
			
			CooldownEffect->DurationMagnitude.AttemptCalculateMagnitude(*EffectSpec.Data.Get(), CooldownDuration);
			
		}
	}
	return FMath::Abs(CooldownDuration);
	
}

float UCAbilitySystemStatics::GetCooldownRemainingFor(const UGameplayAbility* AbilityCDO,
	const UAbilitySystemComponent& ASC, int AbilityLevel)
{
	 // 安全检查：确保技能CDO有效
    if (!AbilityCDO) 
        return 0.0f;

    // 从技能中获取冷却效果（GameplayEffect）
    // GetCooldownGameplayEffect()：返回技能配置的冷却效果
    UGameplayEffect* CooldownEffect = AbilityCDO->GetCooldownGameplayEffect();
    
    // 检查冷却效果是否存在
    if (!CooldownEffect) 
        return 0.0f;

    // 创建冷却效果查询条件
    // FGameplayEffectQuery：用于查询特定GameplayEffect的结构
    FGameplayEffectQuery CooldownEffectQuery;
    
    // 设置查询条件：只查找指定类型的冷却效果
    // EffectDefinition：指定要查询的GameplayEffect类
    CooldownEffectQuery.EffectDefinition = CooldownEffect->GetClass();

    // 初始化剩余冷却时间
    float CooldownRemaining = 0.0f;

    // 返回所有匹配查询条件的活跃GameplayEffect的剩余时间
    FJsonSerializableArrayFloat CooldownTimeRemainings = ASC.GetActiveEffectsTimeRemaining(CooldownEffectQuery);

    // 遍历所有找到的冷却效果，取最长的剩余时间
    // 处理多个冷却效果叠加的情况（如：延长冷却的debuff）
    for (float Remaining : CooldownTimeRemainings)
    {
        // 选择最大的剩余时间（最严格的冷却限制）
        if (Remaining > CooldownRemaining)
        {
            CooldownRemaining = Remaining;
        }
    }
    
    // 返回最终的剩余冷却时间（秒）
    return CooldownRemaining;
}
