// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CPlayerCharacter.h"

#include "AbilitySystemComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"


ACPlayerCharacter::ACPlayerCharacter()
{
	// 初始化弹簧臂
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	// 将弹簧臂附加到根组件上
	SpringArmComp->SetupAttachment(GetRootComponent());
	// 使用pawn的控制器旋转
	SpringArmComp->bUsePawnControlRotation = true;

	// 初始化摄像机
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	// 将摄像机附加到弹簧臂上
	CameraComp->SetupAttachment(SpringArmComp);

	// 不使用自身的Yaw旋转
	bUseControllerRotationYaw = false;
	// 朝向移动的方向转动
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f);
}

void ACPlayerCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();


	// 获得玩家控制器
	APlayerController* OwningPlayerController = GetController<APlayerController>();
	if (OwningPlayerController)
	{
		// 获得增强输入子系统
		UEnhancedInputLocalPlayerSubsystem* InputSubsystem = OwningPlayerController->GetLocalPlayer()->GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>();
		if (InputSubsystem && IM_GameplayInputMappingContext)
		{
			// 移除映射上下文
			InputSubsystem->RemoveMappingContext(IM_GameplayInputMappingContext);
			// 添加映射上下文，优先级为0
			InputSubsystem->AddMappingContext(IM_GameplayInputMappingContext, 0);
		}
	}
}

void ACPlayerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// 获取到增强输入组件
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhancedInputComponent && IA_Jump)
	{
		// 绑定跳跃输入
		EnhancedInputComponent->BindAction(IA_Jump, ETriggerEvent::Triggered, this, &ACPlayerCharacter::Jump);
		EnhancedInputComponent->BindAction(IA_Jump, ETriggerEvent::Completed, this, &ACPlayerCharacter::StopJumping);

		// 绑定视角移动输入
		EnhancedInputComponent->BindAction(IA_Look, ETriggerEvent::Triggered, this,
		                                   &ACPlayerCharacter::HandleLookInput);

		// 绑定移动输入
		EnhancedInputComponent->BindAction(IA_Move, ETriggerEvent::Triggered, this,
		                                   &ACPlayerCharacter::HandleMoveInput);

		for (const TPair<ECAbilityInputID, UInputAction*>& InputActionPair : GameplayAbilityInputActions)
		{
			EnhancedInputComponent->BindAction(InputActionPair.Value, ETriggerEvent::Triggered, this,
			                                   &ACPlayerCharacter::HandleAbilityInput, InputActionPair.Key);
		}
	}
}




void ACPlayerCharacter::HandleAbilityInput(const FInputActionValue& InputActionValue, ECAbilityInputID InputID)
{
	// 获取输入状态（按下=true，释放=false）
	bool bPressed = InputActionValue.Get<bool>();
	// 如果输入是按下状态
	if (bPressed)
	{
		// 通知AbilitySystemComponent：某个输入ID被按下
		GetAbilitySystemComponent()->AbilityLocalInputPressed((int32)InputID);
	}
	else
	{
		// 通知AbilitySystemComponent：某个输入ID被释放
		GetAbilitySystemComponent()->AbilityLocalInputReleased((int32)InputID);
	}
}

void ACPlayerCharacter::HandleLookInput(const FInputActionValue& InputActionValue)
{
	// 获得 输入的二维向量
	FVector2D LookAxisVector = InputActionValue.Get<FVector2D>();
	// 根据输入向量来添加控制器的俯仰和偏航输入
	AddControllerPitchInput(-LookAxisVector.Y);
	AddControllerYawInput(LookAxisVector.X);
}

void ACPlayerCharacter::HandleMoveInput(const FInputActionValue& InputActionValue)
{
	FVector2D InputVal = InputActionValue.Get<FVector2D>();
	InputVal.Normalize();

	// 移动
	AddMovementInput(GetMoveForwardDirection() * InputVal.Y + GetLookRightDirection() * InputVal.X);
}

FVector ACPlayerCharacter::GetLookRightDirection() const
{
	return CameraComp->GetRightVector(); // 获得摄像机右边的方向
}

FVector ACPlayerCharacter::GetLookForwardDirection() const
{
	return CameraComp->GetForwardVector(); // 获得摄像机前面的方向
}

FVector ACPlayerCharacter::GetMoveForwardDirection() const
{
	// 计算两个向量的叉积  输入两个3D向量，输出一个新的向量，这个新向量同时垂直于原来的两个向量。
	return FVector::CrossProduct(GetLookRightDirection(), FVector::UpVector);
}

void ACPlayerCharacter::OnDeath()
{
	APlayerController* PlayerController = GetController<APlayerController>();
	if (PlayerController)
	{
		DisableInput(PlayerController);
	}
}

void ACPlayerCharacter::OnRespawn()
{
	APlayerController* PlayerController = GetController<APlayerController>();
	if (PlayerController)
	{
		EnableInput(PlayerController);
	}
}
