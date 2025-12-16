// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemWidget.generated.h"

class UPA_ShopItem;
class UItemToolTip;
class UImage;
/**
 * 
 */
UCLASS()
class UItemWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	
	virtual void SetIcon(UTexture2D* IconTexture);

protected:
	// 设置描述组件
	UItemToolTip* SetToolTipWidget(const UPA_ShopItem* Item);

	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ItemIcon;

	// 描述文字
	UPROPERTY(EditDefaultsOnly,Category = "ToolTip")
	TSubclassOf<UItemToolTip> ItemToolTipClass;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual void RightButtonClicked();

	virtual void LeftButtonClicked();
	
};
