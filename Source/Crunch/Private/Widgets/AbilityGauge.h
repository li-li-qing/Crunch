// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "AbilityGauge.generated.h"

class UImage;
class UTextBlock;

// DataTable行数据结构：定义技能UI表现
USTRUCT(BlueprintType)
struct FAbilityWidgetData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<class UGameplayAbility> AbilityClass; // 关联的技能类

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FName AbilityName; // 技能名称（用于显示）
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSoftObjectPtr<UTexture2D> Icon; // 技能图标（软引用）

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FText Description; // 技能描述

	
};

/**
 * 单个技能显示控件（列表项）
 */
UCLASS()
class UAbilityGauge : public UUserWidget,public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	/**
	 * @brief 列表项核心回调：数据绑定时触发
	 * @param ListItemObject 
	 */
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	// 应用DataTable中的UI数据
	void ConfigureWidgetData(const FAbilityWidgetData* WidgetData);

	virtual void NativeConstruct() override;
private:
	// 冷却时间更新的频率（秒），控制UI刷新速度
	UPROPERTY(EditDefaultsOnly,Category = "Cooldown")
	float CooldownUpdateInterval = 0.1f;
	
	// 材质参数名：用于动态设置图标
	UPROPERTY(EditDefaultsOnly,Category="Visual")
	FName IconMaterialParamName = "Icon";

	// 冷却时间的图片(转表的那个值)
	UPROPERTY(EditDefaultsOnly,Category="Visual")
	FName CooldownPercentParamName = "Percent";
	
	// 技能的图标
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Icon;
	
	// 冷却剩余持续时间
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CooldownCounterText;

	// 冷却的时间
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CooldownDurationText;

	// 花费多少蓝量
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CostText;
	// 技能的原型对象（Class Default Object），用于类型比较
	UPROPERTY()
	TObjectPtr<UGameplayAbility> AbilityCDO;

	// 技能提交事件回调函数，当技能成功提交时触发
	void AbilityCommitted(UGameplayAbility* Ability);
	// 开始冷却倒计时显示
	void StartCooldown(float CooldownTimeRemaining,float CooldownDuration);
	// 缓存的技能总冷却时长
	float CachedCooldownDuration;
	// 缓存的剩余冷却时间
	float CachedCooldownTimeRemaining;
	// 冷却结束定时器句柄（单次执行）
	FTimerHandle CooldownTimerHandle;
	// 冷却更新定时器句柄（周期性执行，用于实时更新UI）
	FTimerHandle CooldownTimerUpdateHandle;
	// 整数格式化选项（用于显示整数数值）
	FNumberFormattingOptions WholeNumberFormattingOptions;
	// 两位数格式化选项（用于显示带小数点的数值）
	FNumberFormattingOptions TwoDigitNumberFormattingOptions;
	// 冷却结束时的回调函数
	void CooldownFinished();
	// 冷却时间更新函数，周期性调用以更新剩余时间显示
	void UpdateCooldown();
};
