// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/CCharacter.h"
#include "InputActionValue.h"
#include "GAS/CGameplayAbilityTypes.h"
#include "CPlayerCharacter.generated.h"


class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;

/**
 * 
 */
UCLASS()
class ACPlayerCharacter : public ACCharacter
{
	GENERATED_BODY()

public:
	// 构造函数
	ACPlayerCharacter();

	// 当客户端重新启动时调用
	virtual void PawnClientRestart()override;
	// 设置玩家输入组件
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)override;
protected:


private:
	// 弹簧臂
	// VisibleDefaultsOnly: 仅在默认值面板中可见，不能在实例中修改
	UPROPERTY(VisibleDefaultsOnly,Category = "View")
	TObjectPtr<USpringArmComponent> SpringArmComp;
	// 摄像机
	UPROPERTY(VisibleDefaultsOnly,Category = "View")
	TObjectPtr<UCameraComponent> CameraComp;

	// 增强输入映射上下文
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> IM_GameplayInputMappingContext;

	// 增加 跳跃 的输入行为
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Jump;
	// 增加 移动视角 的输入行为
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Look;
	// 增加 移动 的输入行为
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Move;

	// 能力的输入行为
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TMap<ECAbilityInputID,TObjectPtr<UInputAction>> GameplayAbilityInputActions;
	
	void HandleAbilityInput(const FInputActionValue& InputActionValue,ECAbilityInputID InputID);
	
	// 移动视角
	void HandleLookInput(const FInputActionValue& InputActionValue);
	// 移动
	void HandleMoveInput(const FInputActionValue& InputActionValue);

	// 获得摄像机看向的方向 、 右边的方向、要移动的方向
	FVector GetLookRightDirection()const;
	FVector GetLookForwardDirection()const;
	FVector GetMoveForwardDirection()const;
};
