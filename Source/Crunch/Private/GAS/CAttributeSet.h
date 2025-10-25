// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "CAttributeSet.generated.h"
// 增加以下宏,然后使用ATTRIBUTE_ACCESSORS(UCAttributeSet, Health) 可以自动生成4个函数
// 生成 GetHealthAttribute()
// 生成 GetHealth()
// 生成 SetHealth(float)
// 生成 InitHealth(float)
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
/**
 * @brief GAS提供的属性类,可以在里面定义血量、攻击力等等,并且使用 FGameplayAttributeData 里面就有基础值跟当前值
 */
UCLASS()
class UCAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	// 自动为Health生成4个函数
	ATTRIBUTE_ACCESSORS(UCAttributeSet, Health);
	ATTRIBUTE_ACCESSORS(UCAttributeSet, MaxHealth);
	ATTRIBUTE_ACCESSORS(UCAttributeSet, Mana);
	ATTRIBUTE_ACCESSORS(UCAttributeSet, MaxMana);
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	/**
	 * @brief 在属性被修改前调用
	 * @param Attribute 
	 * @param NewValue 
	 */
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	/**
	 * @brief 在 GameplayEffect的所有 Modifiers（属性修改器）应用完成后触发。
	 * @param Data 
	 */
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
private:
	// 血量
	UPROPERTY(ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	UPROPERTY(ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	// 蓝量
	UPROPERTY(ReplicatedUsing = OnRep_Mana)
	FGameplayAttributeData Mana;
	UPROPERTY(ReplicatedUsing = OnRep_MaxMana)
	FGameplayAttributeData MaxMana;
	
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData& OldValue);
};
