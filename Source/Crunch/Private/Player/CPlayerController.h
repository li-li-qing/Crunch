// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CPlayerController.generated.h"

class UGameplayWidget;
class ACPlayerCharacter;
/**
 * 
 */
UCLASS()
class ACPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void OnPossess(APawn* InPawn) override;

	virtual void AcknowledgePossession(APawn* InPawn) override;

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
