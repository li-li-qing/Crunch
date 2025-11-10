// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/StatsGauge.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
void UStatsGauge::NativePreConstruct()
{
	Super::NativePreConstruct();
	// 设置图标纹理：从配置的IconTexture资源设置图标显示
	Icon->SetBrushFromTexture(IconTexture);
}

void UStatsGauge::NativeConstruct()
{
	Super::NativeConstruct();
	// 设置小数点后数位 = 0
	NumberFormattnigOptions.MaximumFractionalDigits = 0;
	APawn* OwnerPlayerPawn = GetOwningPlayerPawn();
	if (!OwnerPlayerPawn) return;

	UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerPlayerPawn);

	if (OwnerASC)
	{
		bool bFound;
		// 获取当前属性值
		// GetGameplayAttributeValue：从GAS组件查询指定属性的当前值
		// Attribute：要查询的属性（如生命值、魔法值等）
		// bFound：返回是否成功找到该属性
		float AttributeVal = OwnerASC->GetGameplayAttributeValue(Attribute, bFound);
		// 设置初始显示值
		SetValue(AttributeVal);
	}
}

void UStatsGauge::SetValue(float NewValue)
{
	// 更新UI文本显示
	// FText::AsNumber：将浮点数转换为文本，应用格式选项
	// &NumberFormattnigOptions：引用格式选项（控制小数位、千分位分隔符等）
	AttributeText->SetText(FText::AsNumber(NewValue,&NumberFormattnigOptions));
}

void UStatsGauge::AttributeChanged(const FOnAttributeChangeData& Data)
{
	// 当监听的属性发生变化时调用，更新UI显示
	// Data.NewValue：属性变化后的新值
	SetValue(Data.NewValue);
}
