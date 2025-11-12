// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PA_AbilitySystemGenerics.generated.h"


class UGameplayEffect;
class UGameplayAbility;
/**
 * 存放通用的能力
 */
UCLASS()
class UPA_AbilitySystemGenerics : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:

	FORCEINLINE TSubclassOf<UGameplayEffect> GetFullStatEffect() const {return FullStatEffect;}
	FORCEINLINE TSubclassOf<UGameplayEffect> GetDeathEffect() const {return DeathEffect;}
	FORCEINLINE const TArray<TSubclassOf<UGameplayEffect>>& GetInitialEffects() const {return InitialEffect;}
	FORCEINLINE const TArray<TSubclassOf<UGameplayAbility>>& GetPassiveAbilities() const {return PassiveAbilities;}
	FORCEINLINE const UDataTable* GetBaseStatDataTable() const {return BaseStatDataTable;}
	const FRealCurve* GetExperienceCurve() const;
private:
	// 全部的状态效果
	UPROPERTY(EditAnywhere, Category="Gameplay Effects");
	TSubclassOf<UGameplayEffect> FullStatEffect;
	// 存放死亡的效果
	UPROPERTY(EditAnywhere, Category="Gameplay Effects");
	TSubclassOf<UGameplayEffect> DeathEffect;
	// 存放所有的Effect效果
	UPROPERTY(EditAnywhere, Category="Gameplay Effects");
	TArray<TSubclassOf<UGameplayEffect>> InitialEffect;

	// 存放所有被动的能力
	UPROPERTY(EditAnywhere, Category="Gameplay Ability");
	TArray<TSubclassOf<UGameplayAbility>> PassiveAbilities;
	
	UPROPERTY(EditAnywhere, Category="Base Stats");
	UDataTable* BaseStatDataTable;

	// 曲线的名字
	UPROPERTY(EditDefaultsOnly, Category="Level");
	FName ExperienceRowName = "ExperienceNeededToReachLevel";

	// 曲线图标
	UPROPERTY(EditDefaultsOnly, Category="Level");
	UCurveTable* ExperienceCurveTable;
};
