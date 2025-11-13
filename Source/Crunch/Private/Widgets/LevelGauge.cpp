// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/LevelGauge.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GAS/CHeroAttributeSet.h"

void ULevelGauge::NativeConstruct()
{
	Super::NativeConstruct();
	// 配置数字格式化选项：设置最大小数位数为0（只显示整数）
	NumberFormattingOptions.SetMaximumFractionalDigits(0);
	// 获取拥有此控件的玩家Pawn
	// GetOwningPlayerPawn()：返回控制此UI的玩家角色
	APawn* OwnerPawn = GetOwningPlayerPawn();
	if (!OwnerPawn) return;
	// 从玩家Pawn获取AbilitySystemComponent（GAS组件）
	// UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent()：安全获取ASC组件
	UAbilitySystemComponent* OwnerAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerPawn);
	if (!OwnerAbilitySystemComponent) return;

	OwnerASC = OwnerAbilitySystemComponent;
	// 关键修改：立即执行一次UI更新，确保初始状态正确显示
	// 传入空的FOnAttributeChangeData()，因为这是初始化调用，没有属性变化数据
	UpdateGauge(FOnAttributeChangeData());
	// 注册属性变化委托：监听经验相关属性的变化
	// 当这些属性变化时，自动调用UpdateGauge函数更新UI
	OwnerAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCHeroAttributeSet::GetExperienceAttribute()).AddUObject(this, &ULevelGauge::UpdateGauge);
	OwnerAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCHeroAttributeSet::GetNextLevelExperienceAttribute()).AddUObject(this, &ULevelGauge::UpdateGauge);
	OwnerAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCHeroAttributeSet::GetPrevLevelExperienceAttribute()).AddUObject(this, &ULevelGauge::UpdateGauge);
	OwnerAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCHeroAttributeSet::GetLevelAttribute()).AddUObject(this, &ULevelGauge::UpdateGauge);
		 
	
}

void ULevelGauge::UpdateGauge(const FOnAttributeChangeData& Data)
{
	bool bFound;
	// 获取当前经验值
	// GetGameplayAttributeValue()：从ASC获取属性的当前值
	float CurrentExperience = OwnerASC->GetGameplayAttributeValue(UCHeroAttributeSet::GetExperienceAttribute(), bFound);
	if (!bFound) return;
    
	// 获取下一级所需经验值
	float NextLevelExperience = OwnerASC->GetGameplayAttributeValue(UCHeroAttributeSet::GetNextLevelExperienceAttribute(), bFound);
	if (!bFound) return;
    
	// 获取当前等级所需经验值
	float PrevLevelExperience = OwnerASC->GetGameplayAttributeValue(UCHeroAttributeSet::GetPrevLevelExperienceAttribute(), bFound);
	if (!bFound) return;
    
	// 获取当前等级
	float CurrentLevel = OwnerASC->GetGameplayAttributeValue(UCHeroAttributeSet::GetLevelAttribute(), bFound);
	if (!bFound) return;

	// 更新等级文本显示
	// SetText()：设置文本内容
	// FText::AsNumber()：将数字转换为文本，应用格式化选项
	LevelText->SetText(FText::AsNumber(CurrentLevel, &NumberFormattingOptions));

	// 计算当前等级内的经验进度
	// Progress：当前等级已获得的经验值
	float Progress = CurrentExperience - PrevLevelExperience;
    
	// 计算当前等级总经验需求
	// LevelExpAmt：升到下一级所需的经验总量
	float LevelExpAmt = NextLevelExperience - PrevLevelExperience;

	// 计算经验进度百分比（0.0 - 1.0）
	float Percent = Progress / LevelExpAmt;

	// 关键修改：处理满级情况
	// 如果NextLevelExperience为0，表示已达到最大等级
	if (NextLevelExperience == 0)
	{
		Percent = 1; // 满级时显示100%进度
	}
    
	// 更新进度条显示
	if (LevelProgressImage)
	{
		// GetDynamicMaterial()：获取图像使用的动态材质实例
		// SetScalarParameterValue()：设置材质的标量参数值
		// 控制进度条的填充百分比（0%=空，100%=满）
		LevelProgressImage->GetDynamicMaterial()->SetScalarParameterValue(PercentMaterialParamName, Percent);
	}
	
}
