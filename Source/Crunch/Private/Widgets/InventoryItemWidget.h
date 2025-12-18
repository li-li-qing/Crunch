// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/ItemWidget.h"
#include "InventoryItemWidget.generated.h"

struct FInventoryItemHandle;
class UTextBlock;
class UInventoryItem;

/**
 * 库存物品插槽控件
 * 用于显示背包中的单个物品，包含物品图标、堆叠数量等信息
 */
UCLASS()
class UInventoryItemWidget : public UItemWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativeConstruct() override;
	
	/**
	 * @brief 检查插槽是否为空
	 * @return 如果插槽为空（无有效物品）则返回true
	 */
	bool IsEmpty() const;

	/**
	 * @brief 设置插槽号
	 * @param NewSlotNumber 
	 */
	void SetSlotNumber(int NewSlotNumber);
	
	/**
	 * @brief 更新物品的数据到自身
	 * @param Item 
	 */
	void UpdateInventoryItem(const UInventoryItem* Item);
	
	/**
	 * @brief 清空插槽
	 * 重置所有属性并将插槽设置为空状态
	 */
	void EmptySlot();
	
	/**
	 * @brief 获取插槽编号
	 * @return 插槽的编号
	 */
	FORCEINLINE int GetSlotNumber() const { return SlotNumber; }


private:
	/**
	 * @brief 更新堆叠的数量
	 */
	void UpdateStackCount();
	
	// 空插槽材质
	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	UTexture2D* EmptyTexture;
	
	// 堆叠数量显示文本
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> StackCountText;
	
	// 冷却时间计数文本
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CooldownCountText;
	
	// 冷却持续时间文本
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CooldownDurationText;
	
	// 魔法值消耗文本
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ManaCostText;
	
	// 关联的库存物品实例
	UPROPERTY()
	const UInventoryItem* InventoryItem;
	
	// 插槽编号
	int SlotNumber;
};