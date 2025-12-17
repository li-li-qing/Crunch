// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/ItemToolTip.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Inventory/PA_ShopItem.h"

void UItemToolTip::SetItem(const UPA_ShopItem* Item)
{
	// 设置名字
	ItemTitleText->SetText(Item->GetItemName());
	// 设置描述
	ItemDescriptionText->SetText(Item->GetItemDescription());
	// 设置价格
	ItemPriceText->SetText(FText::AsNumber((int)Item->GetPrice()));
	// 设置图片
	IconImage->SetBrushFromTexture(Item->GetIcon());
}

void UItemToolTip::SetPrice(float NewPrice)
{
	// 设置价格
	ItemPriceText->SetText(FText::AsNumber((int)NewPrice));
}
