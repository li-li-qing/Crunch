// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/AbilityGauge.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "GAS/CAbilitySystemStatics.h"
#include "GAS/CAttributeSet.h"
#include "GAS/CHeroAttributeSet.h"

void UAbilityGauge::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	// 调用父类的实现，确保基础功能正常
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	// 将传入的数据对象转换为GameplayAbility类
	// Cast<UGameplayAbility>()：安全类型转换，失败时返回nullptr
	AbilityCDO = Cast<UGameplayAbility>(ListItemObject);

	// 从技能的Class Default Object（CDO）获取静态冷却时间
	// UCAbilitySystemStatics::GetStaticCooldownDurationForAbility()：获取技能的基础冷却时间
	float CooldownDuration = UCAbilitySystemStatics::GetStaticCooldownDurationForAbility(AbilityCDO);
    
	// 从技能的CDO获取静态消耗值
	// UCAbilitySystemStatics::GetStaticCostForAbility()：获取技能的基础资源消耗
	float Cost = UCAbilitySystemStatics::GetStaticCostForAbility(AbilityCDO);

	// 设置冷却时间文本显示
	// SetText()：设置文本控件的内容
	// FText::AsNumber()：将数字转换为本地化的文本格式
	CooldownDurationText->SetText(FText::AsNumber(CooldownDuration));
    
	// 设置消耗值文本显示
	CostText->SetText(FText::AsNumber(Cost));

	// 初始化等级仪表显示为0级
	// GetDynamicMaterial()：获取图像使用的动态材质实例
	// SetScalarParameterValue()：设置材质的标量参数值
	LevelGauge->GetDynamicMaterial()->SetScalarParameterValue(AbilityLevelParamName, 0);


}

void UAbilityGauge::ConfigureWidgetData(const FAbilityWidgetData* WidgetData)
{
	// 验证组件和数据有效性
	if (Icon && WidgetData)
	{
		// 获取动态材质实例（用于运行时修改）
		UMaterialInstanceDynamic* DynamicMaterial = Icon->GetDynamicMaterial();
        
		if (DynamicMaterial)
		{
			// 同步加载图标纹理（软引用→硬引用）
			// LoadSynchronous()：将软引用转换为硬引用，加载纹理资源
			UTexture2D* IconTexture = WidgetData->Icon.LoadSynchronous();
            
			// 设置材质参数：将纹理应用到材质
			// SetTextureParameterValue()：设置材质的纹理参数
			DynamicMaterial->SetTextureParameterValue(IconMaterialParamName, IconTexture);
		}
	}
}

void UAbilityGauge::NativeConstruct()
{
	Super::NativeConstruct();
    
	// 初始化冷却文本为隐藏状态
	CooldownCounterText->SetVisibility(ESlateVisibility::Hidden);

	// 获取拥有此控件的玩家的AbilitySystemComponent
	// GetAbilitySystemComponent()：从玩家Pawn获取GAS组件
	UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerPawn());
    
	if (OwnerASC)
	{
		// 注册技能提交事件监听器
		// AbilityCommittedCallbacks：当技能成功提交（资源消耗、冷却检查通过）时触发
		// AddUObject()：将对象方法绑定为回调
		OwnerASC->AbilityCommittedCallbacks.AddUObject(this, &UAbilityGauge::AbilityCommitted);

		// 绑定技能规格数据更新事件
		// AbilitySpecDirtiedCallbacks：当技能规格数据变化时触发（如等级变化）
		OwnerASC->AbilitySpecDirtiedCallbacks.AddUObject(this, &UAbilityGauge::AbilitySpecUpdated);

		// 监听升级点数属性变化
		// GetGameplayAttributeValueChangeDelegate()：获取属性变化委托
		// AddUObject()：绑定属性变化回调
		OwnerASC->GetGameplayAttributeValueChangeDelegate(UCHeroAttributeSet::GetUpgradePointAttribute())
			.AddUObject(this, &UAbilityGauge::UpgradePointUpdated);

		// 监听魔法值属性变化
		OwnerASC->GetGameplayAttributeValueChangeDelegate(UCAttributeSet::GetManaAttribute())
			.AddUObject(this, &UAbilityGauge::ManaUpdated);
        
		// 获取当前升级点数并触发初始更新
		bool bFound = false;
		float UpgradePoint = OwnerASC->GetGameplayAttributeValue(UCHeroAttributeSet::GetUpgradePointAttribute(), bFound);
		if (bFound)
		{
			// 创建属性变化数据并触发更新
			FOnAttributeChangeData ChangeData;
			ChangeData.NewValue = UpgradePoint;
			UpgradePointUpdated(ChangeData);
		}
	}
    
	// 保存ASC引用供后续使用
	OwnerAbilitySystemComponent = OwnerASC;
    
	// 配置数字格式化选项
	// WholeNumberFormattingOptions：整数格式化设置（无小数位）
	WholeNumberFormattingOptions.MaximumFractionalDigits = 0;  // 最大小数位数为0（只显示整数）
    
	// TwoDigitNumberFormattingOptions：两位小数格式化设置
	TwoDigitNumberFormattingOptions.MaximumFractionalDigits = 2; // 最大小数位数为2（显示2位小数）
}

void UAbilityGauge::AbilityCommitted(UGameplayAbility* Ability)
{
	// 检查提交的技能是否与当前UI跟踪的技能匹配
	// GetDefaultObject()：获取类的默认对象（CDO）
	if (Ability->GetClass()->GetDefaultObject() == AbilityCDO)
	{
		float CooldownTimeRemaining = 0.0f;  // 剩余冷却时间
		float CooldownDuration = 0.0f;       // 总冷却时长
        
		// 获取技能的冷却时间信息（剩余时间和总时长）
		// GetCooldownTimeRemainingAndDuration()：获取冷却时间详细信息
		Ability->GetCooldownTimeRemainingAndDuration(
			Ability->GetCurrentAbilitySpecHandle(),    // 当前技能句柄
			Ability->GetCurrentActorInfo(),             // 当前角色信息
			CooldownTimeRemaining,                      // 输出：剩余时间
			CooldownDuration                           // 输出：总时长
		);
        
		// 开始冷却倒计时显示
		StartCooldown(CooldownTimeRemaining, CooldownDuration);
	}
}

void UAbilityGauge::StartCooldown(float CooldownTimeRemaining, float CooldownDuration)
{
	
	CooldownDurationText->SetText(FText::AsNumber(CooldownDuration));// 显示总冷却时间
	// 缓存冷却时间数据
	CachedCooldownDuration = CooldownDuration;
	CachedCooldownTimeRemaining = CooldownTimeRemaining;

	// 显示冷却计数器
	CooldownCounterText->SetVisibility(ESlateVisibility::Visible);
	// 设置冷却结束定时器（单次执行）
	GetWorld()->GetTimerManager().SetTimer(CooldownTimerHandle,this,&UAbilityGauge::CooldownFinished,CachedCooldownTimeRemaining);
	// 设置冷却更新定时器（周期性执行，用于实时更新显示）
	GetWorld()->GetTimerManager().SetTimer(CooldownTimerUpdateHandle,this,&UAbilityGauge::UpdateCooldown,CooldownUpdateInterval,true,0.f);
}

void UAbilityGauge::CooldownFinished()
{
	// 重置冷却时间缓存
	CachedCooldownDuration = CachedCooldownTimeRemaining = 0.f;
	// 隐藏冷却计数器
	CooldownCounterText->SetVisibility(ESlateVisibility::Hidden);
	// 清除冷却更新定时器
	GetWorld()->GetTimerManager().ClearTimer(CooldownTimerUpdateHandle);

	// 设置冷却时间转表的那个值
	Icon->GetDynamicMaterial()->SetScalarParameterValue(CooldownPercentParamName,1.f);
}

void UAbilityGauge::UpdateCooldown()
{

	// 更新剩余冷却时间
	CachedCooldownTimeRemaining-=CooldownUpdateInterval;
	FNumberFormattingOptions* FormattingOptions = CachedCooldownTimeRemaining > 1 ?&WholeNumberFormattingOptions : & TwoDigitNumberFormattingOptions;
	// 更新UI显示
	CooldownCounterText->SetText(FText::AsNumber(CachedCooldownTimeRemaining,FormattingOptions));

	// 设置冷却时间转表的那个值
	Icon->GetDynamicMaterial()->SetScalarParameterValue(CooldownPercentParamName,1.f - CachedCooldownTimeRemaining / CachedCooldownDuration);
}

const FGameplayAbilitySpec* UAbilityGauge::GetAbilitySpec()
{
	// 如果缓存为空，则查找并缓存技能规格
	if (!CachedAbilitySpec)
	{
		if (AbilityCDO && OwnerAbilitySystemComponent)
		{
			// 根据技能类查找技能规格
			// FindAbilitySpecFromClass()：通过技能类查找规格信息
			CachedAbilitySpec = OwnerAbilitySystemComponent->FindAbilitySpecFromClass(AbilityCDO->GetClass());
		}
	}

	return CachedAbilitySpec;
}

void UAbilityGauge::AbilitySpecUpdated(const FGameplayAbilitySpec& AbilitySpec)
{
	// 检查是否是当前UI跟踪的技能
	if (AbilitySpec.Ability != AbilityCDO)
	{
		return;
	}

	// 更新技能学习状态：等级>0表示已学习
	bIsAbilityLearned = (AbilitySpec.Level > 0);
    
	// 更新等级仪表显示
	LevelGauge->GetDynamicMaterial()->SetScalarParameterValue(AbilityLevelParamName, AbilitySpec.Level);
    
	// 更新施法能力状态
	UpdateCanCast();

	// 获取当前等级的冷却时间和消耗
	float NewCooldownDuration = UCAbilitySystemStatics::GetCooldownDurationFor(
		AbilitySpec.Ability, 
		*OwnerAbilitySystemComponent, 
		AbilitySpec.Level
	);
    
	float NewCost = UCAbilitySystemStatics::GetManaCostFor(
		AbilitySpec.Ability, 
		*OwnerAbilitySystemComponent, 
		AbilitySpec.Level
	);

	// 更新UI显示
	CooldownDurationText->SetText(FText::AsNumber(NewCooldownDuration));
	CostText->SetText(FText::AsNumber(NewCost));
}

void UAbilityGauge::UpdateCanCast()
{
	// 获取技能规格
	const FGameplayAbilitySpec* AbilitySpec = GetAbilitySpec();
    
	// 基础条件：技能必须已学习
	bool bCanCast = bIsAbilityLearned;
    
	if (AbilitySpec)
	{
		// 检查资源消耗是否足够
		// CheckAbilityCost()：验证技能消耗条件
		if (OwnerAbilitySystemComponent && !UCAbilitySystemStatics::CheckAbilityCost(*AbilitySpec, *OwnerAbilitySystemComponent))
		{
			bCanCast = false; // 资源不足，不能施放
		}
	}
    
	// 更新材质显示：根据施法能力状态设置参数
	// 1.0=可以施放，0.0=不能施放
	Icon->GetDynamicMaterial()->SetScalarParameterValue(CanCastAbilityParamName, bCanCast ? 1.0f : 0.0f);
}

void UAbilityGauge::UpgradePointUpdated(const FOnAttributeChangeData& Data)
{
	// 检查是否有升级点数可用
	bool HasUpgradePoint = (Data.NewValue > 0.0f);
    
	// 获取技能规格
	const FGameplayAbilitySpec* AbilitySpec = GetAbilitySpec();

	if (AbilitySpec)
	{
		// 检查技能是否已达到最大等级
		// IsAbilityAtMaxLevel()：判断技能是否满级
		if (UCAbilitySystemStatics::IsAbilityAtMaxLevel(*AbilitySpec))
		{
			// 满级技能不显示升级提示
			Icon->GetDynamicMaterial()->SetScalarParameterValue(UpgradePointAvailableParamName, 0.0f);
			return;
		}
	}

	// 更新材质显示：根据升级点数可用性设置参数
	// 1.0=有升级点数可用，0.0=无升级点数
	Icon->GetDynamicMaterial()->SetScalarParameterValue(UpgradePointAvailableParamName, HasUpgradePoint ? 1.0f : 0.0f);
}

void UAbilityGauge::ManaUpdated(const FOnAttributeChangeData& Data)
{
	UpdateCanCast();
}