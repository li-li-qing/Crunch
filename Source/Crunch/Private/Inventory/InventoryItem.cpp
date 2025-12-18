#include "Inventory/InventoryItem.h"
#include "AbilitySystemComponent.h"

#include "PA_ShopItem.h"

/**
 * @brief 默认构造函数
 * 创建一个无效的句柄，ID被设置为0
 */
FInventoryItemHandle::FInventoryItemHandle()
    : HandleId{GetInvalidId()}  // 将ID设置为无效值（0）
{
}

/**
 * @brief 返回一个无效句柄
 * @return 返回一个静态的无效句柄实例
 * 使用静态变量确保每次返回的是同一个无效句柄
 */
FInventoryItemHandle FInventoryItemHandle::InvalidHandle()
{
    static FInventoryItemHandle InvalidHandle = FInventoryItemHandle();
    return InvalidHandle;
}

/**
 * @brief 私有构造函数
 * @param Id 要设置的ID值
 * 只有类内部可以调用这个构造函数
 */
FInventoryItemHandle::FInventoryItemHandle(uint32 Id)
    : HandleId{Id}
{
}

/**
 * @brief 创建一个新的有效句柄
 * @return 返回一个具有新唯一ID的句柄
 * 这是创建新句柄的唯一公开方式
 */
FInventoryItemHandle FInventoryItemHandle::CreateHandle()
{
    return FInventoryItemHandle(GenerateNextId());
}

/**
 * @brief 检查句柄是否有效
 * @return 如果ID不等于0（无效值）则返回true
 */
bool FInventoryItemHandle::IsValid() const
{
    return HandleId != GetInvalidId();
}

/**
 * @brief 生成下一个可用的ID
 * @return 返回一个新的唯一ID
 * 
 * 关键点：
 * 1. static uint32 StaticId = 1; 静态局部变量，只在第一次调用时初始化为1
 * 2. 后续每次调用，StaticId都会保持上一次的值
 * 3. StaticId++ 是后置递增，先返回当前值，然后加1
 * 4. 这意味着：第一次调用返回1，第二次返回2，第三次返回3...
 * 5. 这保证了整个程序运行期间ID是唯一且递增的
 */
uint32 FInventoryItemHandle::GenerateNextId()
{
    static uint32 StaticId = 1;  // 静态变量，初始值为1
    return StaticId++;           // 后置递增，先返回值再加1
}

/**
 * @brief 获取无效ID的值
 * @return 返回0，表示无效ID
 */
uint32 FInventoryItemHandle::GetInvalidId()
{
    return 0;
}

/**
 * @brief 重载相等操作符
 * @param Lhs 左侧比较对象
 * @param Rhs 右侧比较对象
 * @return 如果两个句柄的ID相等则返回true
 */
bool operator==(const FInventoryItemHandle& Lhs, const FInventoryItemHandle& Rhs)
{
    return Lhs.GetHandleId() == Rhs.GetHandleId();
}

/**
 * @brief 获取句柄的哈希值
 * @param Key 要获取哈希的句柄
 * @return 返回句柄的ID作为哈希值
 * 这个函数使得FInventoryItemHandle可以在TSet<T>和TMap<T>中作为键使用
 */
uint32 GetTypeHash(const FInventoryItemHandle& Key)
{
    return Key.GetHandleId();
}

UInventoryItem::UInventoryItem()
    :StackCount{1}
{
    
}

bool UInventoryItem::IsValid() const
{
    return ShopItem != nullptr;
}

/**
 * @brief 初始化库存物品
 * @param NewHandle 物品的唯一句柄
 * @param NewShopItem 对应的商店物品数据
 * 将句柄和商店物品数据绑定到这个库存物品实例
 */
void UInventoryItem::InitItem(const FInventoryItemHandle& NewHandle, const UPA_ShopItem* NewShopItem)
{
    Handle = NewHandle;      // 设置唯一标识
    ShopItem = NewShopItem;  // 设置物品数据
}

void UInventoryItem::ApplyGasModifications(UAbilitySystemComponent* AbilitySystemComponent)
{
    // 安全检查：确保商店物品和能力系统组件有效
    if (!GetShopItem() || !AbilitySystemComponent) return;

    // 权限检查：确保只有服务器端（有Authority）才能执行GAS修改
    if (!AbilitySystemComponent->GetOwner() || !AbilitySystemComponent->GetOwner()->HasAuthority()) return;
    
    // 获取物品的装备效果（EquipEffect）
    TSubclassOf<UGameplayEffect> EquipEffect = GetShopItem()->GetEquippedEffect();
    // 如果物品有装备效果
    if (EquipEffect)
    {
        // 将效果应用到自身
        // 参数说明：
        // 1. EquipEffect: 要应用的GameplayEffect类
        // 2. 1: 效果等级（Level），这里使用默认等级1
        // 3. AbilitySystemComponent->MakeEffectContext(): 创建效果上下文
        AppliedEquippedEffectHandle = AbilitySystemComponent->BP_ApplyGameplayEffectToSelf(
            EquipEffect, 
            1, 
            AbilitySystemComponent->MakeEffectContext()
        );
    }
    // 获取物品授予的能力
    TSubclassOf<UGameplayAbility> GrantedAbility = GetShopItem()->GetGrantedAbility();
    // 如果物品有能力要授予
    if (GrantedAbility)
    {
        // 首先检查玩家是否已经拥有这个能力
        // 这是为了防止重复授予相同的能力
        const FGameplayAbilitySpec* FoundSpec = AbilitySystemComponent->FindAbilitySpecFromClass(GrantedAbility);
        
        if (FoundSpec)
        {
            // 如果已经拥有这个能力，保存现有的能力句柄
            // 这样可以在物品移除时正确管理能力
            GrantedAbilitySpecHandle = FoundSpec->Handle;
        }
        else
        {
            // 如果还没有这个能力，授予新能力
            // 创建能力规格（FGameplayAbilitySpec），包含能力的类和其他设置
            FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(GrantedAbility);
            
            // 授予能力并保存返回的句柄
            GrantedAbilitySpecHandle = AbilitySystemComponent->GiveAbility(AbilitySpec);
        }
    }
        
}

void UInventoryItem::SetSlot(int NewSlot)
{
    Slot = NewSlot;
}
