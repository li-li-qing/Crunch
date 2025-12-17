// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemToolTip.generated.h"

class UPA_ShopItem;
class UTextBlock;
class UImage;
/**
 * 商城的物品的提示框
 */
UCLASS()
class UItemToolTip : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetItem(const UPA_ShopItem* Item);
	void SetPrice(float NewPrice);
private:

	// 图片
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> IconImage;

	// 名字
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemTitleText;

	// 描述
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemDescriptionText;

	// 价格
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemPriceText;
	
};
