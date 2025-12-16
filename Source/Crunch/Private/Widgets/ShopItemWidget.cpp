// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/ShopItemWidget.h"
#include "Inventory/PA_ShopItem.h"

/**
 * @brief 设置列表项对象，绑定商店物品数据
 * @param ListItemObject 列表项对象
 */
void UShopItemWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	// 调用基类实现
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	// 尝试将传入对象转换为商店物品
	ShopItem = Cast<UPA_ShopItem>(ListItemObject);
	if (!ShopItem)  
	{
		// 转换失败，不进行后续操作
		return;
	}
	
	// 使用商店物品的图标设置UI
	SetIcon(ShopItem->GetIcon());

	// 对这个物品设置描述物品
	SetToolTipWidget(ShopItem);
}