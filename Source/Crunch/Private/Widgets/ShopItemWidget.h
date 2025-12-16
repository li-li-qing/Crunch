// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Widgets/ItemWidget.h"
#include "ShopItemWidget.generated.h"

class UPA_ShopItem;

/**
 * @brief 商店物品UI组件，用于在商店中显示单个物品
 */
UCLASS()
class UShopItemWidget : public UItemWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
	
public:
	
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
};