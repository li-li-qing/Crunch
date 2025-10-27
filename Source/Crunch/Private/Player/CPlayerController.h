// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GenericTeamAgentInterface.h"
#include "CPlayerController.generated.h"

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
private:
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
};
