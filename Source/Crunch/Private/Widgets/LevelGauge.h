// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayEffectTypes.h"
#include "LevelGauge.generated.h"

class UTextBlock;
class UImage;
/**
 * 等级仪表板UI控件：显示角色等级和经验进度
 * 功能：实时显示当前等级、经验条进度
 */
UCLASS()
class ULevelGauge : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

private:
	// 材质参数名称：用于控制进度条显示百分比
	// 对应材质中的标量参数，控制进度条填充
	UPROPERTY(EditDefaultsOnly,Category = "Visual")
	FName PercentMaterialParamName = "Percent";

	// 绑定到UI中的进度条图像组件
	// 使用材质显示经验进度条
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> LevelProgressImage;
	// 绑定到UI中的等级文本组件
	// 显示当前等级数字
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> LevelText;
	// 数字格式化选项：控制文本显示格式
	FNumberFormattingOptions NumberFormattingOptions;
	// 指向拥有者的AbilitySystemComponent指针
	// 用于获取角色属性和监听属性变化
	const class UAbilitySystemComponent* OwnerASC;
	// 属性变化回调函数：当经验或等级属性变化时更新UI
	void UpdateGauge(const FOnAttributeChangeData& Data);
};
