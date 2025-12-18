#include "Inventory/InventoryComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "PA_ShopItem.h"
#include "GAS/CHeroAttributeSet.h"
#include "Inventory/InventoryItem.h"

/**
 * @brief 构造函数
 * 初始化组件
 */
UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = true;  // 启用每帧更新
}

/**
 * @brief 开始游戏时调用
 * 获取所有者的能力系统组件
 */
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// 获取所有者的能力系统组件
	OwnerAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
}

/**
 * @brief 尝试购买物品
 * @param ItemToPurchase 要购买的物品
 * 客户端调用，触发服务器RPC进行验证和购买
 */
void UInventoryComponent::TryPurchase(const UPA_ShopItem* ItemToPurchase)
{
	// 检查能力系统组件是否有效
	if (!OwnerAbilitySystemComponent)
	{
		return;
	}
	// 调用服务器RPC进行购买
	Server_Purchase(ItemToPurchase);
}

/**
 * @brief 获得金币数量
 * @return 当前金币数量
 * 从能力系统组件中获取玩家的金币属性
 */
float UInventoryComponent::GetGold() const
{
	bool bFound = false;  // 标志是否找到属性
	if (OwnerAbilitySystemComponent)
	{
		// 从能力系统组件获取金币属性
		float Gold = OwnerAbilitySystemComponent->GetGameplayAttributeValue(
			UCHeroAttributeSet::GetGoldAttribute(),  // 金币属性
			bFound                                   // 是否成功找到
		);
		if (bFound)
		{
			return Gold;
		}
	}
	return 0.f;  // 默认返回0
}

/**
 * @brief 物品插槽变化通知
 * @param Handle 物品的唯一句柄
 * @param NewSlotNumber 新的插槽编号
 * 当UI中的物品位置发生变化时调用
 */
void UInventoryComponent::ItemSlotChanged(const FInventoryItemHandle& Handle, int NewSlotNumber)
{
	// 通过句柄查找物品
	if (UInventoryItem* FoundItem = GetInventoryItemByHandle(Handle))
	{
		// 更新物品的插槽编号
		FoundItem->SetSlot(NewSlotNumber);
	}
}

/**
 * @brief 通过句柄获取库存物品
 * @param Handle 物品的唯一句柄
 * @return 对应的库存物品，如果不存在返回nullptr
 */
UInventoryItem* UInventoryComponent::GetInventoryItemByHandle(const FInventoryItemHandle& Handle) const
{
	// 在映射表中查找物品
	UInventoryItem* const* FoundItem = InventoryMap.Find(Handle);
	if (FoundItem)
	{
		return *FoundItem;
	}
	return nullptr;
}

/**
 * @brief 授予物品给玩家
 * @param NewItem 新的物品
 * 只在服务器权限下执行
 */
void UInventoryComponent::GrantItem(const UPA_ShopItem* NewItem)
{
	// 检查是否有服务器权限
	if (!GetOwner()->HasAuthority())
	{
		return;  // 客户端不执行物品创建逻辑
	}

	// 创建新的库存物品对象
	UInventoryItem* InventoryItem = NewObject<UInventoryItem>();

	// 创建新的唯一句柄（生成新的ID）
	FInventoryItemHandle NewHandle = FInventoryItemHandle::CreateHandle();

	// 初始化物品
	InventoryItem->InitItem(NewHandle, NewItem);

	// 添加到库存映射表
	InventoryMap.Add(NewHandle, InventoryItem);

	// 广播物品添加事件
	OnItemAdded.Broadcast(InventoryItem);

	// 日志输出（服务器端）
	UE_LOG(LogTemp, Warning, TEXT("Server Adding Shop Item:%s, with Id:%d"),
		*(InventoryItem->GetShopItem()->GetItemName().ToString()),
		NewHandle.GetHandleId());

	// 通知客户端物品添加
	Client_ItemAdded(NewHandle, NewItem);
	
	// 应用游戏能力
	InventoryItem->ApplyGasModifications(OwnerAbilitySystemComponent);
}

/**
 * @brief 服务器RPC验证函数
 * @param ItemToPurchase 要购买的物品
 * @return 是否通过验证
 * 在Server_Purchase_Implementation之前调用
 * 可以添加各种验证逻辑（如防作弊检查）
 */
bool UInventoryComponent::Server_Purchase_Validate(const UPA_ShopItem* ItemToPurchase)
{
	// 这里可以添加各种验证逻辑
	// 例如：检查物品是否合法、玩家状态是否正常等
	return true;  // 目前总是返回true，表示通过验证
}

/**
 * @brief 服务器端购买物品RPC实现
 * @param ItemToPurchase 要购买的物品
 * Server_开头的函数是RPC的实现
 */
void UInventoryComponent::Server_Purchase_Implementation(const UPA_ShopItem* ItemToPurchase)
{
	// 检查物品是否有效
	if (!ItemToPurchase)
	{
		return;
	}

	// 检查金币是否足够
	if (GetGold() < ItemToPurchase->GetPrice())
	{
		return;  // 金币不足，购买失败
	}

	// 从玩家金币中扣除物品价格
	OwnerAbilitySystemComponent->ApplyModToAttribute(
		UCHeroAttributeSet::GetGoldAttribute(),  // 目标属性：金币
		EGameplayModOp::Additive,                 // 修改方式：加法
		-ItemToPurchase->GetPrice()               // 修改值：扣除物品价格（负值）
	);

	// 授予物品给玩家
	GrantItem(ItemToPurchase);
}

/**
 * @brief 客户端物品添加RPC实现
 * @param AssignedHandle 服务器分配的物品句柄
 * @param Item 物品数据
 * Client_开头的函数是RPC的实现
 */
void UInventoryComponent::Client_ItemAdded_Implementation(FInventoryItemHandle AssignedHandle, const UPA_ShopItem* Item)
{
	// 检查是否有服务器权限（防止服务器也执行这个函数）
	if (GetOwner()->HasAuthority())
	{
		return;  // 服务器不执行客户端的逻辑
	}

	// 在客户端也创建物品对象
	UInventoryItem* InventoryItem = NewObject<UInventoryItem>();

	// 使用服务器分配的句柄初始化物品
	// 注意：这里使用的是服务器传来的句柄，不是新创建的！
	InventoryItem->InitItem(AssignedHandle, Item);

	// 添加到客户端的库存映射表
	InventoryMap.Add(AssignedHandle, InventoryItem);

	// 广播物品添加事件
	OnItemAdded.Broadcast(InventoryItem);

	// 日志输出（客户端）
	UE_LOG(LogTemp, Warning, TEXT("Client Adding Shop Item:%s, with Id:%d"),
		*(InventoryItem->GetShopItem()->GetItemName().ToString()),
		AssignedHandle.GetHandleId());
}