// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

class UAbilitySystemComponent;
class UPA_ShopItem;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UInventoryComponent();

	/**
	 * @brief 尝试购买物品
	 * @param ItemToPurchase 要购买的物品 
	 */
	void TryPurchase(const UPA_ShopItem* ItemToPurchase);

	/**
	 * @brief 获得金币数量
	 * @return 
	 */
	float GetGold() const;
protected:

	virtual void BeginPlay() override;

private:	
	UPROPERTY()
	UAbilitySystemComponent* OwnerAbilitySystemComponent;

	/********************************************************/
	/*						服务器							*/
	/********************************************************/

	UFUNCTION()
	void Server_Purchase(const UPA_ShopItem* ItemToPurchase);

};
