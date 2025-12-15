// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PA_ShopItem.generated.h"

// 前向声明
class UPA_ShopItem;
class UGameplayAbility;
class UGameplayEffect;

/**
 * @struct FItemCollection
 * @brief 商店物品集合，用于管理一组UPA_ShopItem物品
 */
USTRUCT(BlueprintType)
struct FItemCollection
{
	GENERATED_BODY()
public:
	/**
	 * @brief 默认构造函数
	 */
	FItemCollection();
	
	/**
	 * @brief 带初始物品列表的构造函数
	 * @param Initems 初始物品数组
	 */
	FItemCollection(const TArray<const UPA_ShopItem*>& Initems);
	
	/**
	 * @brief 向集合中添加物品
	 * @param NewItem 要添加的新物品
	 * @param bAddUnique 是否只添加不重复的物品（默认为false）
	 */
	void AddItem(const UPA_ShopItem* NewItem,bool bAddUnique = false);
	
	/**
	 * @brief 检查集合中是否包含指定物品
	 * @param Item 要检查的物品
	 * @return 如果包含返回true，否则返回false
	 */
	bool Contains(const UPA_ShopItem* Item) const;
	
	/**
	 * @brief 获取集合中的所有物品
	 * @return 物品数组的常量引用
	 */
	const TArray<const UPA_ShopItem*>& GetItems() const;
	
private:
	// 存储物品的数组
	TArray<const UPA_ShopItem*> Items;
};

/**
 * @class UPA_ShopItem
 * @brief 商店物品数据资产，用于定义游戏中可购买/使用的物品属性
 */
UCLASS()
class UPA_ShopItem : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/**
	 * @brief 获取主资产ID
	 * @return 资产的唯一标识符
	 */
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

	/**
	 * @brief 获取商店物品的资产类型
	 * @return 资产类型标识符
	 */
	static FPrimaryAssetType GetShopItemAssetType();

	/**
	 * @brief 获取物品图标
	 * @return 物品的纹理图标
	 */
	UTexture2D* GetIcon() const;

	/**
	 * @brief 获取物品名称
	 * @return 物品名称文本
	 */
	FText GetItemName() const {return ItemName;}

	/**
	 * @brief 获取物品描述
	 * @return 物品描述文本
	 */
	FText GetItemDescription() const {return ItemDescription;}

	/**
	 * @brief 获取物品购买价格
	 * @return 购买价格
	 */
	float GetPrice() const {return Price;}

	/**
	 * @brief 获取物品出售价格（默认是购买价格的一半）
	 * @return 出售价格
	 */
	float GetSellPrice() const {return Price/2.f;}

	/**
	 * @brief 获取装备时触发的GameplayEffect
	 * @return GameplayEffect类
	 */
	TSubclassOf<UGameplayEffect> GetEquippedEffect() const {return EquippedEffect;}
	
	/**
	 * @brief 获取消耗时触发的GameplayEffect
	 * @return GameplayEffect类
	 */
	TSubclassOf<UGameplayEffect> GetConsumeEffect() const {return ConsumedEffect;}
	
	/**
	 * @brief 获取物品授予的GameplayAbility
	 * @return GameplayAbility类
	 */
	TSubclassOf<UGameplayAbility> GetGrantedAbility() const {return GrantedAbility;}
	
	/**
	 * @brief 检查物品是否可堆叠
	 * @return 如果可堆叠返回true，否则返回false
	 */
	bool GetIsStackable() const {return bIsStackable;}
	
	/**
	 * @brief 检查物品是否可消耗
	 * @return 如果可消耗返回true，否则返回false
	 */
	bool GetIsConsumable() const {return bIsConsumable;}

	/**
	 * @brief 获取最大堆叠数量
	 * @return 最大堆叠数
	 */
	int GetMaxStackCount() const {return MaxStackCount;}

	/**
	 * @brief 获取合成该物品所需的材料列表
	 * @return 材料物品的软引用数组
	 */
	const TArray<TSoftObjectPtr<UPA_ShopItem>> & GetIngredients() const {return IngredientItems;}
	
private:
	// 物品图标
	UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
	TSoftObjectPtr<UTexture2D> Icon;

	// 物品购买价格
	UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
	float Price;

	// 物品显示名称
	UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
	FText ItemName;

	// 物品描述文本
	UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
	FText ItemDescription;

	// 标记物品是否可消耗
	UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
	bool bIsConsumable;

	// 装备时触发的GameplayEffect
	UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
	TSubclassOf<UGameplayEffect> EquippedEffect;

	// 消耗时触发的GameplayEffect
	UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
	TSubclassOf<UGameplayEffect> ConsumedEffect;

	// 物品授予的GameplayAbility
	UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
	TSubclassOf<UGameplayAbility> GrantedAbility;

	// 标记物品是否可堆叠
	UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
	bool bIsStackable = false;

	// 最大堆叠数量（仅在bIsStackable为true时有效）
	UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
	int MaxStackCount = 5;

	// 合成该物品所需的材料物品列表
	UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
	TArray<TSoftObjectPtr<UPA_ShopItem>> IngredientItems;
};