// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory/InventoryItem.h"
#include "InventoryWidget.generated.h"

class UInventoryItem;
struct FInventoryItemHandle;
class UWrapBox;
class UInventoryItemWidget;

/**
 * 背包UI控件
 * 显示玩家背包中的所有物品
 */
UCLASS()
class UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	/**
	 * @brief 重写的NativeConstruct函数
	 * 初始化背包UI
	 */
	virtual void NativeConstruct() override;
	
private:
	// 物品列表容器
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWrapBox> ItemList;
	
	// 物品插槽控件类
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<UInventoryItemWidget> ItemWidgetClass;
	
	// 背包组件引用
	UPROPERTY()
	class UInventoryComponent* InventoryComponent;
	
	// 所有物品插槽控件的数组
	UPROPERTY()
	TArray<UInventoryItemWidget*> ItemWidgets;
	
	// 物品与对应插槽的映射表
	UPROPERTY()
	TMap<FInventoryItemHandle, UInventoryItemWidget*> PopulatedItemEntryWidgets;
	
	/**
	 * @brief 物品添加回调函数
	 * @param InventoryItem 添加的物品
	 */
	UFUNCTION()
	void ItemAdded(const UInventoryItem* InventoryItem);
	
	/**
	 * @brief 获取下一个可用的空插槽
	 * @return 可用的物品插槽控件，如果没有返回nullptr
	 */
	UInventoryItemWidget* GetNextAvailableSlot() const;
};