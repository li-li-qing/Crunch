// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Widgets/ItemWidget.h"
#include "ShopItemWidget.generated.h"

class UShopItemWidget;
class UPA_ShopItem;

// 当购买的时候广播
DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemPurchaseIssued,const UPA_ShopItem*);
// 当选择物品的时候广播
DECLARE_MULTICAST_DELEGATE_OneParam(FOnShopItemSelected,const UShopItemWidget*);



/**
 * @brief 商店物品UI组件，用于在商店中显示单个物品
 */
UCLASS()
class UShopItemWidget : public UItemWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
	
public:
	FOnItemPurchaseIssued OnItemPurchaseIssued;
	FOnShopItemSelected OnShopItemClicked;
	
	/**
	 * @brief 当列表项对象被设置时的回调
	 * @param ListItemObject 列表项对象
	 */
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	
	/**
	 * @brief 获取绑定的商店物品
	 * @return 商店物品指针
	 */
	FORCEINLINE const UPA_ShopItem* GetShopItem() const { return ShopItem; }
	
private:
	// 绑定的商店物品数据
	UPROPERTY()
	const UPA_ShopItem* ShopItem;

	virtual void RightButtonClicked() override;

	virtual void LeftButtonClicked() override;
};