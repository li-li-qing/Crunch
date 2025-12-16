// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/ShopWidget.h"
#include "Components/TileView.h"
#include "Widgets/ShopItemWidget.h"
#include "Framework/CAssetManager.h"

/**
 * @brief 初始化商店UI
 */
void UShopWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	// 设置为可聚焦
	SetIsFocusable(true);
	
	// 加载商店物品
	LoadShopItems();
	
	// 绑定UI组件生成事件
	ShopItemList->OnEntryWidgetGenerated().AddUObject(this, &UShopWidget::ShopItemWidgetGenerated);
}

/**
 * @brief 异步加载商店物品数据
 */
void UShopWidget::LoadShopItems()
{
	UCAssetManager::Get().LoadShopItems(FStreamableDelegate::CreateUObject(
		this, 
		&UShopWidget::ShopItemLoadFinished
	));
}

/**
 * @brief 商店物品加载完成后的处理
 */
void UShopWidget::ShopItemLoadFinished()
{
	// 获取已加载的商店物品列表
	TArray<const UPA_ShopItem*> ShopItems;
	UCAssetManager::Get().GetLoadedShopItems(ShopItems);
	
	// 将每个商店物品添加到TileView中
	for (const UPA_ShopItem* ShopItem : ShopItems)
	{
		
		ShopItemList->AddItem(const_cast<UPA_ShopItem*>(ShopItem));
	}
}

/**
 * @brief 当商店物品UI组件生成时的回调函数
 * @param NewWidget 新生成的UI组件
 */
void UShopWidget::ShopItemWidgetGenerated(UUserWidget& NewWidget)
{
	// 转换为商店物品UI组件
	UShopItemWidget* ItemWidget = Cast<UShopItemWidget>(&NewWidget);
	if (ItemWidget)
	{
		// 建立商店物品与对应UI组件的映射关系
		ItemsMap.Add(ItemWidget->GetShopItem(), ItemWidget);
	}
}