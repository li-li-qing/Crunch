// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GenericTeamAgentInterface.h"
#include "CCharacter.generated.h"


class UAIPerceptionStimuliSourceComponent;
class UCAttributeSet;
class UCAbilitySystemComponent;
class UWidgetComponent;

UCLASS()
class ACCharacter : public ACharacter, public IAbilitySystemInterface, public IGenericTeamAgentInterface
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
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	/********************************************************/
	/*						队伍ID  							*/
	/********************************************************/
public:
	/** Assigns Team Agent to given TeamID */
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;

	/** Retrieve team identifier in form of FGenericTeamId */
	virtual FGenericTeamId GetGenericTeamId() const override;

private:
	UPROPERTY(ReplicatedUsing = OnRep_TeamId)
	FGenericTeamId TeamId;

	UFUNCTION()
	virtual void OnRep_TeamId() ;

protected:
	virtual void BeginPlay() override;

private:
	// 能力组件
	UPROPERTY(VisibleDefaultsOnly, Category = "Gameplay Ability")
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
	UPROPERTY(VisibleDefaultsOnly, Category = "UI")
	UWidgetComponent* OverHeadWidgetComponent;
	// 多长时间检测一次
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	float HeadStatGaugeVisibilityCheckUpdateGap{1};
	// 可见的范围
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	float HeadStatGaugeVisibilityRangeSquared{10000000.f};
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
public:
	/**
	 * @brief 是否死亡
	 * @return true = 死亡 false = 没死
	 */
	bool IsDead() const;
	/**
	 * @brief 移除所有带有死亡标签的GameplayEffect
	 */
	void RespawnImmediately();
private:
	
	/**
	 * @brief 尝试获取到这个死亡状态,如果获取到了,进行回调函数
	 */
	void BindGASChangeDelegates();
	/**
	 * @brief 获得死亡标签更新后的回调函数
	 * @param Tag 
	 * @param NewCount 
	 */
	void DeathTagUpdated(const FGameplayTag Tag, int32 NewCount);
	UPROPERTY(EditDefaultsOnly, Category = "Death")
	float DeathMontageFinishTimeShift = -0.8f;
	UPROPERTY(EditDefaultsOnly, Category = "Death")
	UAnimMontage* DeathMontage;
	FTransform MeshRelativeTransform;
	FTimerHandle DeathMontageTimerHandle;
	// 当死亡蒙太奇播放完成的时候调用
	void DeathMontageFinished();
	// 播放死亡蒙太奇
	void PlayDeathAnimation();
	/**
	 * @brief 开启布娃娃状态
	 * @param bIsEnabled 
	 */
	void SetRagdollEnabled(bool bIsEnabled);
	void StartDeathSequence();
	void Respawn();
	virtual void OnDeath();
	virtual void OnRespawn();

	/********************************************************/
	/*					AI	  								*/
	/********************************************************/
private:
	/**
	 * @brief 设置AI对自身是否是有效检测目标
	 * @param bIsEnabled 
	 */
	void SetAIPerceptionStimuliSourceEnabled(bool bIsEnabled);
	
	// 用于标记哪些 Actor 能够被 AI 感知系统检测到
	TObjectPtr<UAIPerceptionStimuliSourceComponent> PerceptionStimuliSourceComponent;
	
};
