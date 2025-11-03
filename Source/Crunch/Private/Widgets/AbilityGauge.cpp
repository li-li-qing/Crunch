// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/AbilityGauge.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "GAS/CAbilitySystemStatics.h"
void UAbilityGauge::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
	AbilityCDO = Cast<UGameplayAbility>(ListItemObject);

	float CooldownDuration = UCAbilitySystemStatics::GetStaticCooldownDurationForAbility(AbilityCDO);
	float Cost = UCAbilitySystemStatics::GetStaticCostForAbility(AbilityCDO);

	CooldownDurationText->SetText(FText::AsNumber(CooldownDuration));
	CostText->SetText(FText::AsNumber(Cost));

	WholeNumberFormattingOptions.MaximumFractionalDigits = 0;
	TwoDigitNumberFormattingOptions.MaximumFractionalDigits = 2;
}

void UAbilityGauge::ConfigureWidgetData(const FAbilityWidgetData* WidgetData)
{
	//  验证组件和数据有效性
	if (Icon && WidgetData)
	{
		//  获取动态材质实例（用于运行时修改）
		UMaterialInstanceDynamic* DynamicMaterial = Icon->GetDynamicMaterial();
        
		if (DynamicMaterial)
		{
			//  同步加载图标纹理（软引用→硬引用）
			UTexture2D* IconTexture = WidgetData->Icon.LoadSynchronous();
            
			// 设置材质参数
			DynamicMaterial->SetTextureParameterValue(IconMaterialParamName, IconTexture);
		}
	}
}

void UAbilityGauge::NativeConstruct()
{
	Super::NativeConstruct();
	//  初始化冷却文本为隐藏状态
	CooldownCounterText->SetVisibility(ESlateVisibility::Hidden);

	UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerPawn());
	if (OwnerASC)
	{
		// 注册技能提交事件监听器  当任何技能成功提交（资源消耗、冷却检查通过）时，自动调用指定的回调函数。
		OwnerASC->AbilityCommittedCallbacks.AddUObject(this,&UAbilityGauge::AbilityCommitted);
	}
	
}

void UAbilityGauge::AbilityCommitted(UGameplayAbility* Ability)
{
	// 检查提交的技能是否与当前UI跟踪的技能匹配
	if (Ability->GetClass()->GetDefaultObject() == AbilityCDO)
	{
		float CooldownTimeRemaining = 0.f;  // 剩余冷却时间
		float CooldownDuration = 0.f; // 总冷却时长
		// 获取技能的冷却时间信息（剩余时间和总时长）
		Ability->GetCooldownTimeRemainingAndDuration(Ability->GetCurrentAbilitySpecHandle(),Ability->GetCurrentActorInfo(),CooldownTimeRemaining,CooldownDuration);
		// 开始冷却倒计时显示
		StartCooldown(CooldownTimeRemaining,CooldownDuration);
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
