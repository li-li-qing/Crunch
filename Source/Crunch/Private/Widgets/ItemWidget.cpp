// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/ItemWidget.h"
#include "ItemToolTip.h"
#include "Components/Image.h"

void UItemWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetIsFocusable(true);
}

void UItemWidget::SetIcon(UTexture2D* IconTexture)
{
	ItemIcon->SetBrushFromTexture(IconTexture);
}

UItemToolTip* UItemWidget::SetToolTipWidget(const UPA_ShopItem* Item)
{
	if (!Item)
	{
		return nullptr;
	}

	if (GetOwningPlayer() && ItemToolTipClass)
	{
		UItemToolTip* ToolTip = CreateWidget<UItemToolTip>(GetOwningPlayer(),ItemToolTipClass);
		if (ToolTip)
		{
			ToolTip->SetItem(Item);
			// 设置描述文件
			SetToolTip(ToolTip);
		}
		return ToolTip;
	}
	return nullptr;
}

/**
 * @brief 处理鼠标按钮按下事件
 * @param InGeometry 小部件的几何信息
 * @param InMouseEvent 鼠标事件信息
 * @return 事件处理回复，指示事件是否被处理
 * 
 * 此函数处理鼠标在物品小部件上的按下操作：
 * 1. 左键按下：设置焦点并开始拖拽检测
 * 2. 右键按下：仅设置焦点
 * 3. 其他情况：调用父类处理
 */
FReply UItemWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// 调用父类处理，保留父类的默认行为
	FReply SuperReply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	
	// 处理右键按下事件
	if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
	{
		// 右键按下时设置焦点但不开始拖拽
		return FReply::Handled()						// 标记事件已处理
			.SetUserFocus(TakeWidget());				// 将焦点设置到此小部件
	}
	
	// 处理左键按下事件
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		// 左键按下时设置焦点并开始拖拽检测
		return FReply::Handled()						// 标记事件已处理
			.SetUserFocus(TakeWidget())					// 将焦点设置到此小部件
			.DetectDrag(								// 开始检测拖拽操作
				TakeWidget(),							// 拖拽的目标小部件
				EKeys::LeftMouseButton					// 触发拖拽的按键
			);
	}

	// 其他按键或情况，返回父类的处理结果
	return SuperReply;
}

/**
 * @brief 处理鼠标按钮释放事件
 * @param InGeometry 小部件的几何信息
 * @param InMouseEvent 鼠标事件信息
 * @return 事件处理回复，指示事件是否被处理
 * 
 * 此函数处理鼠标在物品小部件上的释放操作：
 * 1. 只在有焦点时处理（确保是本小部件的点击）
 * 2. 右键释放：触发右键点击事件
 * 3. 左键释放：触发左键点击事件
 * 4. 其他情况：调用父类处理
 */
FReply UItemWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// 调用父类处理，保留父类的默认行为
	FReply SuperReply = Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);

	// 只有当此小部件拥有焦点时才处理点击事件
	// 这可以防止在A上按下鼠标，拖到B上释放时错误地触发B的点击事件
	if (HasAnyUserFocus())
	{
		// 处理右键释放事件
		if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
		{
			// 触发右键点击的回调函数
			RightButtonClicked();
			
			// 标记事件已处理
			return FReply::Handled();
		}
		
		// 处理左键释放事件
		if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
		{
			// 触发左键点击的回调函数
			LeftButtonClicked();
			
			// 标记事件已处理
			return FReply::Handled();
		}
	}
	
	// 如果没有焦点或不是左右键，返回父类的处理结果
	return SuperReply;
}

void UItemWidget::RightButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Right Mouse button clicked"));
}

void UItemWidget::LeftButtonClicked()
{
	UE_LOG(LogTemp,Warning, TEXT("Left Mouse button clicked"));
}
