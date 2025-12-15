// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "CGameplayAbilityTypes.generated.h"

/**
 * @brief 技能的类型ID
 */
UENUM(BlueprintType)
enum class ECAbilityInputID: uint8
{
	None			UMETA(DisplayName = "None"),
	BasicAttack		UMETA(DisplayName = "Basic Attack"),
	AbilityOne		UMETA(DisplayName = "Ability One"),
	AbilityTwo		UMETA(DisplayName = "Ability Two"),
	AbilityThree	UMETA(DisplayName = "Ability Three"),
	AbilityFour		UMETA(DisplayName = "Ability Four"),
	AbilityFive		UMETA(DisplayName = "Ability Five"),
	AbilitySix		UMETA(DisplayName = "Ability Six"),
	Confirm			UMETA(DisplayName = "Confirm"),
	Cancel			UMETA(DisplayName = "Cancel"),
};

USTRUCT(BlueprintType)
struct FGenericDamageEffectDef
{
	GENERATED_BODY()
	FGenericDamageEffectDef();
public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayEffect> DamageEffect;

	UPROPERTY(EditAnywhere)
	FVector PushVelocity;
};

USTRUCT(BlueprintType)
struct FHeroBaseStats : public FTableRowBase
{
	GENERATED_BODY()

	FHeroBaseStats();
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> Class;

	// 力量
	UPROPERTY(EditAnywhere)
	float Strength;

	// 智力
	UPROPERTY(EditAnywhere)
	float Intelligence;

	// 力量的成长率
	UPROPERTY(EditAnywhere)
	float StrengthGrowthRate;

	// 智力的成长率
	UPROPERTY(EditAnywhere)
	float IntelligenceGrowthRate;

	// 基础的最大血量
	UPROPERTY(EditAnywhere)
	float BaseMaxHealth;

	// 基础的最大蓝量
	UPROPERTY(EditAnywhere)
	float BaseMaxMana;

	// 基础的攻击力
	UPROPERTY(EditAnywhere)
	float BaseAttackDamage;

	// 基础的防御力
	UPROPERTY(EditAnywhere)
	float BaseArmor;

	// 基础的移动速度
	UPROPERTY(EditAnywhere)
	float BaseMoveSpeed;
	
};
