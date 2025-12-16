// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GAS/CGameplayAbilityTypes.h"
#include "GameplayWidget.generated.h"


class UShopWidget;
class UAbilitySystemComponent;
class UValueGauge;
class UAbilityListView;
class UStatsGauge;
/**
 * @brief 主UI界面
 */
UCLASS()
class UGameplayWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	// 配置技能
	void ConfigureAbilities(const TMap<ECAbilityInputID,TSubclassOf<class UGameplayAbility>>& Abilities);

	/**
	 * @brief 切换背包
	 */
	void ToggleShop();
private:
	// 血条
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UValueGauge> HealthBar;
	// 蓝条
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UValueGauge> ManaBar;

	// 技能列表
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UAbilityListView> AbilityListView;

	// 攻击属性
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UStatsGauge> AttackDamageGauge;

	// 防御属性
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UStatsGauge> ArmorGauge;

	// 移动速度属性
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UStatsGauge> MoveSpeedGauge;

	// 智力属性
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UStatsGauge> IntelligenceGauge;

	// 力量
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UStatsGauge> StrengthGauge;

	// 商店
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UShopWidget> ShopWidget;

	// 商店的过渡动画
	UPROPERTY(Transient,meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> ShopPopupAnimation;

	/**
	 * @brief 播放背包的动画
	 * @param bPlayForward true:正序播放 false:逆序播放 
	 */
	void PlayShopPopupAnimation(bool bPlayForward);

	/**
	 * @brief 设置当前Widget拥有者的输入状态
	 * @param bPawnInputEnabled 
	 */
	void SetOwningPawnInputEnabled(bool bPawnInputEnabled);

	/**
	 * @brief 显示鼠标的显示
	 * @param bShowMouseCursor true:显示鼠标 false:隐藏鼠标
	 */
	void SetShowMouseCursor(bool bShowMouseCursor);

	/**
	 * @brief 设置游戏模式为GameAndUI
	 */
	void SetFocusToGameAndUI();
	/**
	 * @brief 设置游戏模式为GameOnly
	 */
	void SetFocusToGameOnly();
	
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> OwnerAbilitySystemComponent;
};
