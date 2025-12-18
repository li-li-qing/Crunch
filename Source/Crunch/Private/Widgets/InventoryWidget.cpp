// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/InventoryWidget.h"
#include "Components/WrapBox.h"
#include "Components/WrapBoxSlot.h"
#include "Inventory/InventoryComponent.h"
#include "Widgets/InventoryItemWidget.h"
#include "Inventory/InventoryItem.h"
void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	// 获取拥有者玩家
	if (APawn* OwnerPawn = GetOwningPlayerPawn())
	{
		// 获取背包组件
		InventoryComponent = OwnerPawn->GetComponentByClass<UInventoryComponent>();
		if (InventoryComponent)
		{
			// 绑定物品添加委托
			InventoryComponent->OnItemAdded.AddUObject(this, &UInventoryWidget::ItemAdded);
			
			// 获取背包容量
			int Capacity = InventoryComponent->GetCapacity();
			
			// 清空现有的UI元素
			ItemList->ClearChildren();
			
			// 根据背包容量创建对应数量的插槽
			for (int i = 0; i < Capacity; i++)
			{
				// 创建新的物品插槽控件
				UInventoryItemWidget* NewEmptyWidget = CreateWidget<UInventoryItemWidget>(GetOwningPlayer(), ItemWidgetClass);
				if (NewEmptyWidget)
				{
					// 设置插槽编号
					NewEmptyWidget->SetSlotNumber(i);
					
					// 将控件添加到WrapBox容器中
					UWrapBoxSlot* NewItemSlot = ItemList->AddChildToWrapBox(NewEmptyWidget);
					
					// 设置插槽间距
					NewItemSlot->SetPadding(FMargin(2.f));
					
					// 添加到插槽数组
					ItemWidgets.Add(NewEmptyWidget);
				}
			}
		}
	}
}

void UInventoryWidget::ItemAdded(const UInventoryItem* InventoryItem)
{
	if (!InventoryItem) return;
	
	// 获取下一个可用的空插槽
	if (UInventoryItemWidget* NextAvailableSlot = GetNextAvailableSlot())
	{
		// 更新插槽显示
		NextAvailableSlot->UpdateInventoryItem(InventoryItem);
		
		// 添加到映射表
		PopulatedItemEntryWidgets.Add(InventoryItem->GetHandle(), NextAvailableSlot);
		
		// 通知背包组件插槽变化
		if (InventoryComponent)
		{
			InventoryComponent->ItemSlotChanged(InventoryItem->GetHandle(), NextAvailableSlot->GetSlotNumber());
		}
	}
}

UInventoryItemWidget* UInventoryWidget::GetNextAvailableSlot() const
{
	// 遍历所有插槽，找到第一个空插槽
	for (UInventoryItemWidget* Widget : ItemWidgets)
	{
		if (Widget->IsEmpty())
		{
			return Widget;
		}
	}
	
	// 没有可用插槽
	return nullptr;
}