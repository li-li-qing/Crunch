// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "Inventory/PA_ShopItem.h"
#include "CAssetManager.generated.h"

/**
 * @class UCAssetManager
 * @brief 自定义资产管理器，扩展UE原生资产管理系统
 * 
 * 负责管理游戏中所有商店物品的加载、缓存和查询
 * 提供物品合成系统的数据支持
 */
UCLASS()
class UCAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	/**
	 * @brief 获取资产管理器的单例实例
	 * @return 资产管理器的引用
	 */
	static UCAssetManager& Get();
	
	/**
	 * @brief 异步加载所有商店物品
	 * @param LoadFinishedCallback 加载完成时执行的回调委托
	 */
	void LoadShopItems(const FStreamableDelegate& LoadFinishedCallback);
	
	/**
	 * @brief 获取已加载的商店物品列表
	 * @param OutItems 输出参数，存储已加载的物品指针数组
	 * @return 如果成功获取物品列表返回true，否则返回false
	 */
	bool GetLoadedShopItems(TArray<const UPA_ShopItem*>& OutItems) const;
	

	
private:
	/**
	 * @brief 商店物品加载完成的回调函数
	 * @param Callback 外部传入的回调委托
	 */
	void ShopItemLoadFinished(FStreamableDelegate Callback);
	
	/**
	 * @brief 构建物品合成关系映射表
	 * 
	 * 初始化 CombinationMap 和 IngredientMap，建立材料与合成物品的双向映射关系
	 */
	void BuildItemMaps();
	
	/**
	 * @brief 将物品添加到合成映射表中
	 * @param Ingredient 材料物品
	 * @param CombinationItem 由该材料合成的物品
	 */
	void AddToCombinationMap(const UPA_ShopItem* Ingredient,const UPA_ShopItem* CombinationItem);
	
	// 映射：材料物品 -> 可合成的物品集合
	// Key: 材料物品，Value: 使用该材料可合成的所有物品
	UPROPERTY()
	TMap<const UPA_ShopItem*,FItemCollection> CombinationMap;
	
	// 映射：合成物品 -> 所需的材料集合
	// Key: 合成物品，Value: 合成该物品所需的所有材料
	UPROPERTY()
	TMap<const UPA_ShopItem*,FItemCollection> IngredientMap;
};


