#include "Framework/CAssetManager.h"

/**
 * @brief 获取资产管理器的单例实例
 * @return 资产管理器的引用
 */
UCAssetManager& UCAssetManager::Get()
{
	// 尝试从全局引擎中获取资产管理器
	UCAssetManager* Singleton = Cast<UCAssetManager>(GEngine->AssetManager.Get());
	if (Singleton)
	{
		return *Singleton;
	}
	
	// 如果无法获取，记录致命错误并创建新实例
	UE_LOG(LogTemp, Fatal, TEXT("Asset Manager Needs to be of the type CAssetManager"));
	return (*NewObject<UCAssetManager>());
}
/**
 * @brief 异步加载所有商店物品
 * @param LoadFinishedCallback 加载完成时执行的回调委托
 * 
 * 通过UE的主资产系统异步加载所有商店物品
 * 加载完成后会调用BuildItemMaps构建物品关系映射
 */
void UCAssetManager::LoadShopItems(const FStreamableDelegate& LoadFinishedCallback)
{
	// 异步加载指定类型的所有主资产
	LoadPrimaryAssetsWithType(
		UPA_ShopItem::GetShopItemAssetType(),	// 要加载的资产类型
		TArray<FName>(),						// 资产包列表（空表示加载所有）
		FStreamableDelegate::CreateUObject(		// 创建回调委托
			this,
			&UCAssetManager::ShopItemLoadFinished,
			LoadFinishedCallback
		)
	);
}

/**
 * @brief 获取已加载的商店物品列表
 * @param OutItems 输出参数，存储已加载的物品指针数组
 * @return 如果成功获取物品列表返回true，否则返回false
 */
bool UCAssetManager::GetLoadedShopItems(TArray<const UPA_ShopItem*>& OutItems) const
{
	// 获取指定类型的主资产对象列表
	TArray<UObject*> LoadedObjects;
	bool bLoaded = GetPrimaryAssetObjectList(
		UPA_ShopItem::GetShopItemAssetType(),	// 资产类型
		LoadedObjects							// 输出：加载的对象数组
	);

	// 如果加载成功，将对象转换为商店物品类型
	if (bLoaded)
	{
		for (UObject* ObjectLoaded : LoadedObjects)
		{
			// 将UObject转换为UPA_ShopItem并添加到输出数组
			OutItems.Add(Cast<UPA_ShopItem>(ObjectLoaded));
		}
	}
	
	return bLoaded;
}

/**
 * @brief 商店物品加载完成的回调函数
 * @param Callback 外部传入的回调委托
 * 
 * 1. 执行外部回调
 * 2. 构建物品合成关系映射
 */
void UCAssetManager::ShopItemLoadFinished(FStreamableDelegate Callback)
{
	// 执行外部回调
	Callback.ExecuteIfBound();
	
	// 构建物品合成关系映射
	BuildItemMaps();
}

/**
 * @brief 构建物品合成关系映射表
 * 
 * 遍历所有已加载的商店物品，构建两个映射表：
 * 1. IngredientMap: 物品 -> 所需材料集合
 * 2. CombinationMap: 材料 -> 可合成物品集合
 * 
 * 这实现了材料与合成物品的双向查找功能
 */
void UCAssetManager::BuildItemMaps()
{
	// 获取所有已加载的商店物品
	TArray<const UPA_ShopItem*> LoadedItems;
	if (GetLoadedShopItems(LoadedItems))
	{
		// 遍历所有物品
		for (const UPA_ShopItem* Item : LoadedItems)
		{
			// 跳过没有材料要求的物品（基础材料）
			if (Item->GetIngredients().Num() == 0)
			{
				continue;
			}
			
			// 获取物品的所有材料
			TArray<const UPA_ShopItem*> Items;
			for (const TSoftObjectPtr<UPA_ShopItem>& Ingredient : Item->GetIngredients())
			{
				// 同步加载材料物品
				UPA_ShopItem* IngredientItem = Ingredient.LoadSynchronous();
				
				// 添加到当前物品的材料列表
				Items.Add(IngredientItem);
				
				// 添加到组合映射表：材料 -> 可合成的物品
				AddToCombinationMap(IngredientItem, Item);
			}
			
			// 添加到材料映射表：物品 -> 所需材料
			IngredientMap.Add(Item, FItemCollection{Items});
		}
	}
}

/**
 * @brief 将物品添加到合成映射表中
 * @param Ingredient 材料物品
 * @param CombinationItem 由该材料合成的物品
 * 
 * 维护CombinationMap映射：
 * - 如果材料已存在映射中，将合成物品添加到其集合中
 * - 如果材料不存在，创建新的映射条目
 */
void UCAssetManager::AddToCombinationMap(const UPA_ShopItem* Ingredient, const UPA_ShopItem* CombinationItem)
{
	// 查找该材料是否已有合成映射
	FItemCollection* Combinations = CombinationMap.Find(Ingredient);
	if (Combinations)
	{
		// 如果映射已存在，检查并添加不重复的合成物品
		if (!Combinations->Contains(CombinationItem))
		{
			// 注意：这里有bug，应该是添加到现有集合，而不是新增条目
			// 应该改为：Combinations->AddItem(CombinationItem, true);
			// 当前代码会错误地添加新条目
			CombinationMap.Add(CombinationItem);
		}
	}
	else
	{
		// 创建新的映射条目
		CombinationMap.Add(
			Ingredient,
			FItemCollection{TArray<const UPA_ShopItem*>{CombinationItem}}
		);
	}
}