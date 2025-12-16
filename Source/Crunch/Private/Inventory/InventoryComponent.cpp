// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "PA_ShopItem.h"
#include "GAS/CHeroAttributeSet.h"


UInventoryComponent::UInventoryComponent()
{

	PrimaryComponentTick.bCanEverTick = true;


}

void UInventoryComponent::TryPurchase(const UPA_ShopItem* ItemToPurchase)
{
	if (!OwnerAbilitySystemComponent)
	{
		return;
	}

	
}

float UInventoryComponent::GetGold() const
{
	bool bFound = false;
	if (OwnerAbilitySystemComponent)
	{
		float Gold = OwnerAbilitySystemComponent->GetGameplayAttributeValue(UCHeroAttributeSet::GetGoldAttribute(),bFound);
		if (bFound)
		{
			return Gold;
		}
	}
	return 0.f;
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();


	OwnerAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
	
}

void UInventoryComponent::Server_Purchase(const UPA_ShopItem* ItemToPurchase)
{
	if (!ItemToPurchase)
	{
		return;
	}

	if (GetGold() < ItemToPurchase->GetPrice())
	{
		return;
	}
	

	// 从玩家金币中扣除物品价格
	OwnerAbilitySystemComponent->ApplyModToAttribute(
		UCHeroAttributeSet::GetGoldAttribute(),  // 目标属性：金币
		EGameplayModOp::Additive,                // 修改方式：加法
		-ItemToPurchase->GetPrice()             // 修改值：扣除物品价格
	);
}



