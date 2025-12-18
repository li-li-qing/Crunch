// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ShopWidget.generated.h"

class UInventoryComponent;
class UTileView;
class UShopItemWidget;
class UPA_ShopItem;

/**
 * @brief 商店UI组件，用于显示和管理商店中的商品
 */
UCLASS()
class UShopWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/**
	 * @brief 重写NativeConstruct，进行初始化操作
	 */
	virtual void NativeConstruct() override;
	
private:
	// 商店物品列表视图
	UPROPERTY(meta = (BindWidget))
	UTileView* ShopItemList;

	/**
	 * @brief 加载商店物品数据
	 */
	void LoadShopItems();
	
	/**
	 * @brief 商店物品加载完成时的回调
	 */
	void ShopItemLoadFinished();
	
	/**
	 * @brief 商店物品UI组件生成时的回调
	 * @param NewWidget 新生成的UI组件
	 */
	void ShopItemWidgetGenerated(UUserWidget& NewWidget);

	// 商店物品与对应UI组件的映射表
	UPROPERTY()
	TMap<const UPA_ShopItem*, const UShopItemWidget*> ItemsMap;

	UPROPERTY()
	UInventoryComponent* OwnerInventoryComponent;
};