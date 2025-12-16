// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/GameplayWidget.h"
#include"AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GAS/CAbilitySystemComponent.h"
#include "ValueGauge.h"
#include "GAS/CAttributeSet.h"
#include "Widgets/AbilityListView.h"
#include "Widgets/ShopWidget.h"

void UGameplayWidget::NativeConstruct()
{
	Super::NativeConstruct();
	// 获取玩家角色的AbilitySystemComponent
	OwnerAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerPawn());

	// 获得所有的技能
	// const UCAbilitySystemComponent* CAbilitySystemComponent = Cast<UCAbilitySystemComponent>(OwnerAbilitySystemComponent);
	// if (CAbilitySystemComponent)
	// {
	// 	ConfigureAbilities(CAbilitySystemComponent->GetAbilities());
	// }

	if (OwnerAbilitySystemComponent)
	{
		// 绑定血条到Health/MaxHealth属性
		HealthBar->SetAndBoundToGameplayAttribute(OwnerAbilitySystemComponent, UCAttributeSet::GetHealthAttribute(),
		                                          UCAttributeSet::GetMaxHealthAttribute());
		// 绑定蓝条到Mana/MaxMana属性
		ManaBar->SetAndBoundToGameplayAttribute(OwnerAbilitySystemComponent, UCAttributeSet::GetManaAttribute(),
		                                        UCAttributeSet::GetMaxManaAttribute());
	}
}

void UGameplayWidget::ConfigureAbilities(const TMap<ECAbilityInputID, TSubclassOf<class UGameplayAbility>>& Abilities)
{
	AbilityListView->ConfigureAbilities(Abilities);
}

void UGameplayWidget::PlayShopPopupAnimation(bool bPlayForward)
{
	if (bPlayForward)
	{
		PlayAnimationForward(ShopPopupAnimation);
		SetOwningPawnInputEnabled(false);
		SetShowMouseCursor(true);
		SetFocusToGameAndUI();
		ShopWidget->SetFocus();
	}
	else
	{
		PlayAnimationReverse(ShopPopupAnimation);
		SetOwningPawnInputEnabled(true);
		SetShowMouseCursor(false);
		SetFocusToGameOnly();
	}
}

void UGameplayWidget::SetOwningPawnInputEnabled(bool bPawnInputEnabled)
{
	if (bPawnInputEnabled)
	{
		GetOwningPlayerPawn()->EnableInput(GetOwningPlayer());
	}
	else
	{
		GetOwningPlayerPawn()->DisableInput(GetOwningPlayer());
	}
}

void UGameplayWidget::SetShowMouseCursor(bool bShowMouseCursor)
{
	GetOwningPlayer()->SetShowMouseCursor(bShowMouseCursor);
}

void UGameplayWidget::SetFocusToGameAndUI()
{
	FInputModeGameAndUI GameAndUIInputMode;
	GameAndUIInputMode.SetHideCursorDuringCapture(false);

	GetOwningPlayer()->SetInputMode(GameAndUIInputMode);
}

void UGameplayWidget::SetFocusToGameOnly()
{
	FInputModeGameOnly GameOnlyInputMode;
	GetOwningPlayer()->SetInputMode(GameOnlyInputMode);
}

void UGameplayWidget::ToggleShop()
{
	if (ShopWidget->GetVisibility() == ESlateVisibility::HitTestInvisible)
	{
		ShopWidget->SetVisibility(ESlateVisibility::Visible);
		PlayShopPopupAnimation(true);
	}
	else
	{
		ShopWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
		PlayShopPopupAnimation(false);
	}
}
