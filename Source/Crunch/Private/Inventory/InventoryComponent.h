// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory/InventoryItem.h"
#include "InventoryComponent.generated.h"

// 前向声明
class UAbilitySystemComponent;
class UPA_ShopItem;

/**
 * @brief 物品添加委托
 * @param NewItem 新添加的物品
 * 当物品被添加到库存时广播
 */
DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemAddedDelegate, const UInventoryItem* /* NewItem */)

/**
 * @brief 库存组件
 * 处理玩家物品的购买、存储和管理
 * 注意：这是一个网络复制的组件，需要在服务器和客户端之间同步
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/**
	 * @brief 构造函数
	 */
	UInventoryComponent();

	/**
	 * @brief 物品添加委托
	 * 当有新物品添加到库存时，会触发这个委托
	 */
	FOnItemAddedDelegate OnItemAdded;

	/**
	 * @brief 尝试购买物品
	 * @param ItemToPurchase 要购买的物品
	 * 客户端调用，会触发服务器RPC
	 */
	void TryPurchase(const UPA_ShopItem* ItemToPurchase);

	/**
	 * @brief 获得金币数量
	 * @return 当前金币数量
	 */
	float GetGold() const;

	/**
	 * @brief 获取背包容量
	 * @return 背包的最大物品数量
	 */
	FORCEINLINE int GetCapacity() const { return Capacity; }

	/**
	 * @brief 物品插槽变化通知
	 * @param Handle 物品的唯一句柄
	 * @param NewSlotNumber 新的插槽编号
	 * 当UI中的物品位置发生变化时调用
	 */
	void ItemSlotChanged(const FInventoryItemHandle& Handle, int NewSlotNumber);

	/**
	 * @brief 通过句柄获取库存物品
	 * @param Handle 物品的唯一句柄
	 * @return 对应的库存物品，如果不存在返回nullptr
	 */
	UInventoryItem* GetInventoryItemByHandle(const FInventoryItemHandle& Handle) const;

protected:
	/**
	 * @brief 开始游戏时调用
	 */
	virtual void BeginPlay() override;

private:
	// 背包容量
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	int Capacity = 6;

	/**
	 * @brief 所有者的能力系统组件
	 * 用于获取和修改玩家的属性（如金币）
	 */
	UPROPERTY()
	UAbilitySystemComponent* OwnerAbilitySystemComponent;

	/**
	 * @brief 库存映射表
	 * Key: 物品句柄（唯一ID）
	 * Value: 库存物品对象
	 * 用于快速查找和管理库存中的物品
	 */
	UPROPERTY()
	TMap<FInventoryItemHandle, UInventoryItem*> InventoryMap;

	/********************************************************/
	/*						服务器逻辑						*/
	/********************************************************/

	/**
	 * @brief 服务器端购买物品RPC
	 * @param ItemToPurchase 要购买的物品
	 * Server标记：只在服务器上执行
	 * Reliable标记：保证可靠传输
	 * WithValidation：有验证函数
	 */
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Purchase(const UPA_ShopItem* ItemToPurchase);

	/**
	 * @brief 授予物品给玩家
	 * @param NewItem 新的物品
	 * 只在服务器权限下执行，创建物品并添加到库存
	 */
	void GrantItem(const UPA_ShopItem* NewItem);

	/********************************************************/
	/*						客户端逻辑						*/
	/********************************************************/
private:
	/**
	 * @brief 客户端物品添加RPC
	 * @param AssignedHandle 服务器分配的物品句柄
	 * @param Item 物品数据
	 * Client标记：只在客户端上执行
	 * Reliable标记：保证可靠传输
	 */
	UFUNCTION(Client, Reliable)
	void Client_ItemAdded(FInventoryItemHandle AssignedHandle, const UPA_ShopItem* Item);
};