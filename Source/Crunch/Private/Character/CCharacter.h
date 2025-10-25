// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "CCharacter.generated.h"


class UCAttributeSet;
class UCAbilitySystemComponent;
class UWidgetComponent;
UCLASS()
class ACCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ACCharacter();
	virtual void PossessedBy(AController* NewController) override;
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	/**
	 * @brief 获得GAS组件
	 * @return 
	 */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	/**
	 * @brief 初始化GAS
	 */
	void Server_SideInit();
	/**
	 * @brief 初始化GAS
	 */
	void Client_SideInit();
	/**
	 * @brief 判断当前角色是否由本地玩家控制
	 * @return 
	 */
	bool IsLocallyControlledByPlayer() const;
protected:
	virtual void BeginPlay() override;

private:

	// 能力组件
	UPROPERTY(VisibleDefaultsOnly,Category = "Gameplay Ability")
	TObjectPtr<UCAbilitySystemComponent> CAbilitySystemComponent;

	// 角色的属性
	UPROPERTY()
	TObjectPtr<UCAttributeSet> CAttributeSet;

	/********************************************************/
	/*					玩家头顶的血条	  						*/
	/********************************************************/
	/**
	 * @brief 配置角色头顶状态控件（血条/蓝条等）
	 */
	void ConfigureOverHeadStatusWidget();
	// 玩家头顶的血条跟蓝条
	UPROPERTY(VisibleDefaultsOnly,Category = "UI")
	UWidgetComponent* OverHeadWidgetComponent;
	// 多长时间检测一次
	UPROPERTY(EditDefaultsOnly,Category = "UI")
	float HeadStatGaugeVisibilityCheckUpdateGap{1};
	// 可见的范围
	UPROPERTY(EditDefaultsOnly,Category = "UI")
	float HeadStatGaugeVisibilityRangeSquared {10000000.f};
	// 定时器,用于检测玩家超出距离隐藏头顶的血条
	FTimerHandle HeadStatGaugeVisibilityUpdateTimerHandle;
	/**
	 * @brief 更新头顶血条的可见性（根据与本地玩家的距离动态显示/隐藏）
	 */
	void UpdateHeadGaugeVisibility();

	/**
	 * @brief 开关头顶的血条 
	 * @param bIsEnabled 
	 */
	void SetStatusGaugeEnabled(bool bIsEnabled);
	/********************************************************/
	/*						 死亡和重生 						*/
	/********************************************************/
	/**
	 * @brief 尝试获取到这个死亡状态,如果获取到了,进行回调函数
	 */
	void BindGASChangeDelegates();
	/**
	 * @brief 获得死亡标签更新后的回调函数
	 * @param Tag 
	 * @param NewCount 
	 */
	void DeathTagUpdated(const FGameplayTag Tag,int32 NewCount);
	UPROPERTY(EditDefaultsOnly,Category = "Death")
	UAnimMontage* DeathMontage;
	// 播放死亡蒙太奇
	void PlayDeathAnimation();
	
	void StartDeathSequence();
	void Respawn();
	virtual void OnDeath();
	virtual void OnRespawn();
};
