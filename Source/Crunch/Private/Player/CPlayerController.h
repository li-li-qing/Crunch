// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GenericTeamAgentInterface.h"
#include "CPlayerController.generated.h"

class UInputAction;
class UInputMappingContext;
class UGameplayWidget;
class ACPlayerCharacter;
/**
 * 
 */
UCLASS()
class ACPlayerController : public APlayerController,public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	virtual void OnPossess(APawn* InPawn) override;

	virtual void AcknowledgePossession(APawn* InPawn) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void SetupInputComponent() override;
	/********************************************************/
	/*						队伍ID  							*/
	/********************************************************/
public:
	/** Assigns Team Agent to given TeamID */
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID)override;
	
	/** Retrieve team identifier in form of FGenericTeamId */
	virtual FGenericTeamId GetGenericTeamId() const override;
private:
	UPROPERTY(Replicated)
	FGenericTeamId TeamId;
	/**
	 * @brief 生成gameplay的UI界面
	 */
	void SpawnGameplayWidget();
	
	UPROPERTY()
	TObjectPtr<ACPlayerCharacter> CPlayerCharacter;
	// GameplayWidget的类,在引擎中选择
	UPROPERTY(EditAnywhere,Category="UI")
	TSubclassOf<UGameplayWidget> GameplayWidgetClass;
	// 在代码中使用的GameplayWidget
	UPROPERTY()
	TObjectPtr<UGameplayWidget> GameplayWidget;

	// 输入上下文
	UPROPERTY(EditDefaultsOnly,Category = "Input")
	TObjectPtr<UInputMappingContext> UIInputMapping;

	// 切换背包的输入IA
	UPROPERTY(EditDefaultsOnly,Category = "Input")
	TObjectPtr<UInputAction> ShopToggleInputAction;

	/**
	 * @brief 切换背包
	 */
	UFUNCTION()
	void ToggleShop();
	
	
};
