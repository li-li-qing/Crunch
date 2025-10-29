// Fill out your copyright notice in the Description page of Project Settings.


#include "ValueGauge.h"
#include "AbilitySystemComponent.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UValueGauge::NativePreConstruct()
{
	Super::NativePreConstruct();
	// 设置填充颜色和不透明度
	ProgressBar->SetFillColorAndOpacity(BarColor);
	ValueText->SetFont(ValueTextFont);

	ValueText->SetVisibility(bValueTextVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	ProgressBar->SetVisibility(bProgressBarVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

void UValueGauge::SetAndBoundToGameplayAttribute(UAbilitySystemComponent* AbilitySystemComponent,
	const FGameplayAttribute& Attribute, const FGameplayAttribute& MaxAttribute)
{
	//初始属性值获取与设置
	if (AbilitySystemComponent)
	{
		// 尝试获取当前属性值
		bool bFound;// 用于接收是否成功找到属性
		float Value = AbilitySystemComponent->GetGameplayAttributeValue(Attribute,bFound);
		float MaxValue = AbilitySystemComponent->GetGameplayAttributeValue(MaxAttribute,bFound);
		// 如果成功获取到属性值
		if (bFound)
		{
			// 调用UI更新方法（设置进度条和文本）
			SetValue(Value,MaxValue);
		}
	}
	// 绑定属性变化委托（核心同步机制）
	// 注册普通属性变化回调
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(this,&ThisClass::ValueChanged);
	// 注册最大值属性变化回调
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(MaxAttribute).AddUObject(this,&ThisClass::MaxValueChanged);
}


void UValueGauge::ValueChanged(const FOnAttributeChangeData& ChangedData)
{
	// ChangedData.NewValue: 属性的新值（如 Health 从 100 → 80）
	// CachedMaxValue: 之前缓存的最大值（如 MaxHealth = 100）
	SetValue(ChangedData.NewValue,CachedMaxValue);
}

void UValueGauge::MaxValueChanged(const FOnAttributeChangeData& ChangedData)
{
	// CachedValue: 之前缓存的当前值（如 Health = 80）
	// ChangedData.NewValue: 最大值的新值（如 MaxHealth 从 100 → 150）
	SetValue(CachedValue,ChangedData.NewValue);
}

void UValueGauge::SetValue(float NewValue, float NewMaxValue)
{
	// 把要变更的值缓存下来
	CachedValue = NewValue;
	CachedMaxValue = NewMaxValue;
	// 检查最大值合法性
	if (NewMaxValue == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Value Guage: %s ,NewMaxValue can't be 0"), *GetName());
		return;
	}
	//  计算进度百分比
	float NewPercent = NewValue / NewMaxValue;
	ProgressBar->SetPercent(NewPercent);
	// 设置数字格式（此处限制小数位数为 0，即显示整数）。
	FNumberFormattingOptions FormatOptions = FNumberFormattingOptions().SetMaximumFractionalDigits(0);
	// 将 NewValue和 NewMaxValue格式化为 "当前值/最大值"（如 "50/100"）。
	ValueText->SetText(FText::Format(
		FTextFormat::FromString("{0}/{1}"),
		FText::AsNumber(NewValue, &FormatOptions),
		FText::AsNumber(NewMaxValue, &FormatOptions)
		)
	);
	
}