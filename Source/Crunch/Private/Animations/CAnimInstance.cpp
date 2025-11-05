// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/CAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GAS/CAbilitySystemStatics.h"


void UCAnimInstance::NativeInitializeAnimation()
{
	// 获得拥有此动画实例的角色
	OwnerCharacter = Cast<ACharacter>(TryGetPawnOwner());
	if (OwnerCharacter)
	{
		// 获得玩家移动组件
		OwnerCharacterMovementComponent = OwnerCharacter->GetCharacterMovement();
	}

	// 获得能力系统
	UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TryGetPawnOwner());
	if (OwnerASC)
	{
		OwnerASC->RegisterGameplayTagEvent(UCAbilitySystemStatics::GetAimStatTag()).AddUObject(this,&UCAnimInstance::OwnerAimTagChanged);
	}
}

void UCAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (OwnerCharacter)
	{
		// 获取到角色的速度
		FVector Velocity = OwnerCharacter->GetVelocity();
		// 获取玩家速度
		Speed = Velocity.Length();

		FRotator BodyRotation = OwnerCharacter->GetActorRotation();

		// 计算这一帧跟上一帧的身体旋转差值  计算两个旋转量之间的标准化差值（单位：角度，范围在 -180 到 180 之间）
		FRotator BodyRotationDelta = UKismetMathLibrary::NormalizedDeltaRotator(BodyRotation, BodyPreviousRotation);

		// 将当前的身体角度赋值给上一帧身体角度，为下一帧做准备
		BodyPreviousRotation = BodyRotation;

		// 旋转速度 = 插值之间的 yaw 值 / 时间差 .计算 Yaw 方向的瞬时角速度（单位：度/秒）
		YawSpeed = BodyRotationDelta.Yaw / DeltaSeconds;

		// 使用线性插值平滑 YawSpeed，避免突变（如摄像机抖动,人物抽风）
		SmoothedYawSpeed = UKismetMathLibrary::FInterpTo(
			SmoothedYawSpeed,      // 当前平滑值
			YawSpeed,              // 目标值（原始角速度）
			DeltaSeconds,          // 时间步长
			YawSpeedSmoothLerpSpeed // 插值速度系数（控制平滑程度）
		);
		// 这里有一个bug,在多人游戏中,左右摇头的Yaw好像没有复制 
		// 获取角色控制器的基础瞄准旋转（ControlRotation）这个旋转代表了角色当前看向的方向
		FRotator ControlRotation = OwnerCharacter->GetBaseAimRotation();
		// 计算头部视角与身体旋转之间的标准化偏移量（LookRotationOffSet）
		// 使用UKismetMathLibrary::NormalizedDeltaRotator函数计算两个旋转之间的最小角度差
		// 参数1: ControlRotation - 当前的瞄准方向
		// 参数2: BodyRotation - 角色的身体基础旋转（通常是角色朝向前方时的旋转）
		// 返回值: 返回一个标准化的旋转差值，确保所有角度都在[-180,180]范围内
		// 这个偏移量将用于控制头部/摄像机相对于身体的额外旋转
		LookRotationOffSet = UKismetMathLibrary::NormalizedDeltaRotator(ControlRotation, BodyRotation);
		// 计算角色在控制方向上的速度分量
		FwdSpeed = Velocity.Dot(ControlRotation.Vector());
		// 叉积计算
		RightSpeed = -Velocity.Dot(ControlRotation.Vector().Cross(FVector::UpVector));
	}
	if (OwnerCharacterMovementComponent)
	{
		// 获得是否跳跃
		bIsJumping = OwnerCharacterMovementComponent->IsFalling();
			
	}
}

void UCAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);
}

bool UCAnimInstance::ShouldDoFullBody() const
{
	return (GetSpeed() <= 0) && !(GetIsAiming());
}

void UCAnimInstance::OwnerAimTagChanged(FGameplayTag Tag, int32 NewCount)
{
	// 当获取到标签的时候,调整瞄准的bool
	bIsAiming = NewCount != 0;
}


