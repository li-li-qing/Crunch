// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "GAS/CAbilitySystemComponent.h"
#include "GAS/CAttributeSet.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/OverHeadStatsGauge.h"
#include "GAS/CAbilitySystemStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

ACCharacter::ACCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	// 设置骨骼网格体不产生碰撞
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// 创建GAS组件
	CAbilitySystemComponent = CreateDefaultSubobject<UCAbilitySystemComponent>(TEXT("CAbilitySystemComponent"));
	CAttributeSet = CreateDefaultSubobject<UCAttributeSet>(TEXT("CAttributeSet"));
	// 创建玩家头顶的血条跟蓝条组件,然后附加到根组件上
	OverHeadWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverHeadWidgetComponent"));
	OverHeadWidgetComponent->SetupAttachment(GetRootComponent());
	BindGASChangeDelegates();
}

void ACCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	// 当有控制器获得这个角色的时候,并且不是玩家,是AI的情况下,给AI初始化一下血量
	if (NewController && !NewController->IsPlayerController())
	{
		Server_SideInit();
	}
}

void ACCharacter::Server_SideInit()
{
	// 初始化GAS的OwnerActor和AvatarActor（均为当前角色）
	CAbilitySystemComponent->InitAbilityActorInfo(this, this);
	// 应用初始GameplayEffect（如初始血量、攻击力Buff等）
	CAbilitySystemComponent->ApplyInitialEffects();
	// 赋予角色初始技能（Gameplay Abilities）
	CAbilitySystemComponent->GiveInitialAbilities();
}

void ACCharacter::Client_SideInit()
{
	// 初始化GAS的OwnerActor和AvatarActor
	CAbilitySystemComponent->InitAbilityActorInfo(this, this);
}

bool ACCharacter::IsLocallyControlledByPlayer() const
{
	return GetController() && GetController()->IsLocalPlayerController();
}

void ACCharacter::BeginPlay()
{
	Super::BeginPlay();

	ConfigureOverHeadStatusWidget();
}

void ACCharacter::BindGASChangeDelegates()
{
	if (CAbilitySystemComponent)
	{
		// 尝试获取到这个死亡状态,如果获取到了,进行回调函数
		CAbilitySystemComponent->RegisterGameplayTagEvent(UCAbilitySystemStatics::GetDeadStatTag()).AddUObject(this,&ACCharacter::DeathTagUpdated);
	}
}

void ACCharacter::DeathTagUpdated(const FGameplayTag Tag, int32 NewCount)
{
	if (NewCount != 0)
	{
		
		StartDeathSequence();
	}
	else
	{
		Respawn();
	}
}

void ACCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

UAbilitySystemComponent* ACCharacter::GetAbilitySystemComponent() const
{
	return CAbilitySystemComponent;
}

void ACCharacter::ConfigureOverHeadStatusWidget()
{
	//检查头顶Widget组件是否存在
	if (!OverHeadWidgetComponent) return;

	if (IsLocallyControlledByPlayer())
	{
		OverHeadWidgetComponent->SetHiddenInGame(true);
		return;
	}
	// 尝试获取Widget组件中的UOverHeadStatsGauge实例  GetUserWidgetObject()返回组件关联的UUserWidget实例
	UOverHeadStatsGauge* OverHeadStatsGauge = Cast<UOverHeadStatsGauge>(OverHeadWidgetComponent->GetUserWidgetObject());
	if (OverHeadStatsGauge)
	{
		// 调用控件的配置方法，传入角色的AbilitySystemComponent 绑定血量跟蓝量到Attbute中
		OverHeadStatsGauge->ConfigureWithASC(GetAbilitySystemComponent());
		OverHeadWidgetComponent->SetHiddenInGame(false);
		GetWorldTimerManager().ClearTimer(HeadStatGaugeVisibilityUpdateTimerHandle);
		GetWorldTimerManager().SetTimer(HeadStatGaugeVisibilityUpdateTimerHandle, this,
		                                &ThisClass::UpdateHeadGaugeVisibility,
		                                HeadStatGaugeVisibilityCheckUpdateGap,
		                                true);
	}
}

void ACCharacter::UpdateHeadGaugeVisibility()
{
	//获取本地玩家Pawn（索引0表示主玩家）
	APawn* LocalPlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);

	if (LocalPlayerPawn)
	{
		// 计算当前角色与本地玩家的距离平方（比直接计算距离更高效）
		float DistSquared = FVector::DistSquared(
			GetActorLocation(), // 当前角色的世界坐标
			LocalPlayerPawn->GetActorLocation() // 本地玩家的世界坐标
		);
		// 设置头顶Widget的可见性：
		OverHeadWidgetComponent->SetHiddenInGame(DistSquared > HeadStatGaugeVisibilityRangeSquared);
	}
}

void ACCharacter::SetStatusGaugeEnabled(bool bIsEnabled)
{
	GetWorldTimerManager().ClearTimer(HeadStatGaugeVisibilityUpdateTimerHandle);
	if (bIsEnabled)
	{
		// 启用头顶的血条
		ConfigureOverHeadStatusWidget();
	}
	else
	{
		// 关闭头顶的血条
		OverHeadWidgetComponent->SetHiddenInGame(true);
	}
}

void ACCharacter::PlayDeathAnimation()
{
	if (DeathMontage)
	{
		PlayAnimMontage(DeathMontage);
	}
	
}

void ACCharacter::StartDeathSequence()
{
	OnDeath();
	// 播放死亡动画
	PlayDeathAnimation();
	// 把头上的血条隐藏
	SetStatusGaugeEnabled(false);
	// 禁用角色的移动
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	// 禁用胶囊的碰撞
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ACCharacter::Respawn()
{
	OnRespawn();
}

void ACCharacter::OnDeath()
{
}

void ACCharacter::OnRespawn()
{
}
