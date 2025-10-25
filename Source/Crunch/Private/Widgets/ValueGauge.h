// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AttributeSet.h"
#include "GameplayEffectTypes.h"
#include "ValueGauge.generated.h"

class UAbilitySystemComponent;
class UTextBlock;
class UProgressBar;
/**
 * @brief 一个进度条跟一个数值,用来制作血条跟蓝条
 */
UCLASS()
class CRUNCH_API UValueGauge : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;
	/**
	 * @brief 将UI控件（ValueGauge）绑定到GAS属性（Attribute），实现自动同步更新
	 * @param AbilitySystemComponent 目标ASC（角色技能系统组件）
	 * @param Attribute 要绑定的属性（如Health）
	 * @param MaxAttribute 对应的最大值属性（如MaxHealth）
	 */
	void SetAndBoundToGameplayAttribute(UAbilitySystemComponent* AbilitySystemComponent,const FGameplayAttribute& Attribute,const FGameplayAttribute& MaxAttribute);
	/**
	 * @brief 根据传入的值更改进度条的数值跟文本的数值
	 * @param NewValue 
	 * @param NewMaxValue 
	 */
	void SetValue(float NewValue,float NewMaxValue);
private:
	// 回调委托 当数值更新的时候调用
	void ValueChanged(const FOnAttributeChangeData& ChangedData);
	void MaxValueChanged(const FOnAttributeChangeData& ChangedData);
	float CachedValue;
	float CachedMaxValue;
	// 进度条的颜色
	UPROPERTY(EditAnywhere,Category="Visual")
	FLinearColor BarColor;
	// 进度条
	UPROPERTY(VisibleAnywhere,meta = (BindWidget))
	TObjectPtr<UProgressBar> ProgressBar;
	// 文本
	UPROPERTY(VisibleAnywhere,meta = (BindWidget))
	TObjectPtr<UTextBlock> ValueText;
};
