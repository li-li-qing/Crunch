// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayAbilitySpecHandle.h"

#include "InventoryItem.generated.h"

struct FActiveGameplayEffectHandle;
class UAbilitySystemComponent;
// 前向声明商店物品类
class UPA_ShopItem;

/**
 * @brief 库存物品句柄结构体，用于唯一标识每个库存物品实例
 * 这是一个非常重要的概念：即使两个物品是相同的商店物品类型，
 * 在库存中它们也是不同的实例，需要不同的ID来区分
 */
USTRUCT()
struct FInventoryItemHandle
{
    GENERATED_BODY()
    
public:
    /**
     * @brief 默认构造函数
     */
    FInventoryItemHandle();
    
    /**
     * @brief 创建一个无效的句柄
     * @return 返回一个ID为0的无效句柄
     */
    static FInventoryItemHandle InvalidHandle();
    
    /**
     * @brief 创建一个新的有效句柄
     * @return 返回一个具有新ID的句柄
     * 注意：每次调用都会生成一个新的唯一ID
     */
    static FInventoryItemHandle CreateHandle();
    
    /**
     * @brief 检查句柄是否有效
     * @return 如果ID不是0则返回true
     */
    bool IsValid() const;
    
    /**
     * @brief 获取句柄的ID值
     * @return 返回uint32类型的ID
     */
    uint32 GetHandleId() const {return HandleId;}
    
private:
    /**
     * @brief 私有构造函数
     * @param Id 要设置的ID
     * 设为私有是为了强制使用CreateHandle()来创建有效句柄
     */
    explicit FInventoryItemHandle(uint32 Id);
    
    // 实际的ID值，使用UPROPERTY()支持序列化
    UPROPERTY()
    uint32 HandleId;
    
    /**
     * @brief 生成下一个可用的ID
     * @return 返回一个新的唯一ID
     * 使用静态变量确保ID在整个程序运行期间唯一递增
     */
    static uint32 GenerateNextId();
    
    /**
     * @brief 获取无效ID的值
     * @return 返回0，表示无效ID
     */
    static uint32 GetInvalidId();
};

/**
 * @brief 重载相等操作符，比较两个句柄是否相等
 * @param Lhs 左侧句柄
 * @param Rhs 右侧句柄
 * @return 如果两个句柄的ID相等则返回true
 */
bool operator==(const FInventoryItemHandle& Lhs, const FInventoryItemHandle& Rhs);

/**
 * @brief 获取句柄的哈希值
 * @param Key 要获取哈希的句柄
 * @return 返回句柄的哈希值
 * 这个函数使得FInventoryItemHandle可以在TSet、TMap中使用
 */
uint32 GetTypeHash(const FInventoryItemHandle& Key);

/**
 * @brief 库存物品类
 * 代表玩家库存中的一个具体物品实例
 */
UCLASS()
class UInventoryItem : public UObject
{
    GENERATED_BODY()
    
public:
    UInventoryItem();
    
    bool IsValid() const;
    
    /**
     * @brief 初始化库存物品
     * @param NewHandle 物品的唯一句柄
     * @param NewShopItem 对应的商店物品数据
     */
    void InitItem(const FInventoryItemHandle& NewHandle, const UPA_ShopItem* NewShopItem);
    
    /**
     * @brief 获取商店物品数据
     * @return 返回指向商店物品的指针
     */
    const UPA_ShopItem* GetShopItem() const {return ShopItem;}
    
    /**
     * @brief 获取物品的句柄
     * @return 返回物品的唯一句柄
     */
    FInventoryItemHandle GetHandle() const {return Handle;}

    /**
     * @brief 应用GAS能力
     */
    void ApplyGasModifications(UAbilitySystemComponent* AbilitySystemComponent);

    FORCEINLINE int GetStackCount() const {return StackCount;}

    /**
     * @brief 设置插槽
     * @param NewSlot 
     */
    void SetSlot(int NewSlot);
private:
    // 指向商店物品的引用，包含物品的公共信息（名称、图标、价格等）
    UPROPERTY()
    const UPA_ShopItem* ShopItem;
    
    // 此物品实例的唯一句柄，用于区分相同类型的多个物品
    UPROPERTY()
    FInventoryItemHandle Handle;

    // 堆叠数量
    int StackCount;
    // 插槽的下标
    int Slot;
    
    FActiveGameplayEffectHandle AppliedEquippedEffectHandle;
    FGameplayAbilitySpecHandle GrantedAbilitySpecHandle;
};