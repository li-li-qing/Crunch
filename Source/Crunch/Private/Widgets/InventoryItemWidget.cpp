// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/InventoryItemWidget.h"

#include "ItemToolTip.h"
#include "Components/TextBlock.h"
#include "Inventory/InventoryItem.h"
#include "Inventory/PA_ShopItem.h"

void UInventoryItemWidget::NativeConstruct()
{
	Super::NativeConstruct();
	EmptySlot();
}

bool UInventoryItemWidget::IsEmpty() const
{
	// 检查物品是否为空或无效
	return !InventoryItem || !(InventoryItem->IsValid());
}

void UInventoryItemWidget::SetSlotNumber(int NewSlotNumber)
{
	SlotNumber = NewSlotNumber;
}

void UInventoryItemWidget::UpdateInventoryItem(const UInventoryItem* Item)
{
	InventoryItem = Item;
	if (!InventoryItem || !InventoryItem->IsValid() || InventoryItem->GetStackCount() <= 0)
	{
		EmptySlot();
		return;
	}
	// 设置图片
	SetIcon(Item->GetShopItem()->GetIcon());
	// 设置描述
	UItemToolTip* ToolTip = SetToolTipWidget(InventoryItem->GetShopItem());
	if (ToolTip)
	{
		// 装备的物品想卖掉,只能卖一半的价格
		ToolTip->SetPrice(InventoryItem->GetShopItem()->GetSellPrice());
		
	}
	if (InventoryItem->GetShopItem()->GetIsStackable())
	{
		StackCountText->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		StackCountText->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UInventoryItemWidget::EmptySlot()
{
	// 清空物品引用
	InventoryItem = nullptr;
	
	// 设置图标为空材质
	SetIcon(EmptyTexture);
	
	// 清除工具提示
	SetToolTip(nullptr);
	
	// 隐藏所有额外信息文本
	StackCountText->SetVisibility(ESlateVisibility::Hidden);
	ManaCostText->SetVisibility(ESlateVisibility::Hidden);
	CooldownCountText->SetVisibility(ESlateVisibility::Hidden);
	CooldownDurationText->SetVisibility(ESlateVisibility::Hidden);
}



void UInventoryItemWidget::UpdateStackCount()
{
	if (InventoryItem)
	{
		// 设置堆叠数量
		StackCountText->SetText(FText::AsNumber(InventoryItem->GetStackCount()));
	}
}
