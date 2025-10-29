// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CCharacter.h"

#include "MovieSceneTracksComponentTypes.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "GAS/CAbilitySystemComponent.h"
#include "GAS/CAttributeSet.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/OverHeadStatsGauge.h"
#include "GAS/CAbilitySystemStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Net/UnrealNetwork.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
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

	PerceptionStimuliSourceComponent = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("PerceptionStimuliSourceComponent"));
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

void ACCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACCharacter,TeamId);
}

void ACCharacter::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	TeamId = NewTeamID;
}

FGenericTeamId ACCharacter::GetGenericTeamId() const
{
	return TeamId;
}

void ACCharacter::OnRep_TeamId()
{
	// 由子类覆盖
}

void ACCharacter::BeginPlay()
{
	Super::BeginPlay();

	ConfigureOverHeadStatusWidget();

	// 获得变换信息
	MeshRelativeTransform = GetMesh()->GetRelativeTransform();
	// 将当前Actor注册为AI视觉感知系统的有效检测目标  参数1:需要注册的感知类型
	PerceptionStimuliSourceComponent->RegisterForSense(UAISense_Sight::StaticClass());
}

void ACCharacter::BindGASChangeDelegates()
{
	if (CAbilitySystemComponent)
	{
		// 尝试获取到这个死亡状态,如果获取到了,进行回调函数
		CAbilitySystemComponent->RegisterGameplayTagEvent(UCAbilitySystemStatics::GetDeadStatTag()).AddUObject(
			this, &ACCharacter::DeathTagUpdated);
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

bool ACCharacter::IsDead() const
{
	return GetAbilitySystemComponent()->HasMatchingGameplayTag(UCAbilitySystemStatics::GetDeadStatTag());
}

void ACCharacter::RespawnImmediately()
{
	if (HasAuthority())
	{
		GetAbilitySystemComponent()->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(UCAbilitySystemStatics::GetDeadStatTag()));
		
	}
}

void ACCharacter::DeathMontageFinished()
{
	// 只有死亡了才开启布娃娃状态
	if (IsDead())
	{
		SetRagdollEnabled(true);
	}
	
}

void ACCharacter::PlayDeathAnimation()
{
	if (DeathMontage)
	{
		// 播放死亡动画蒙太奇并获取动画时长
		float MontageDuration = PlayAnimMontage(DeathMontage);
		GetWorldTimerManager().SetTimer(DeathMontageTimerHandle, this,&ACCharacter::DeathMontageFinished,MontageDuration+DeathMontageFinishTimeShift);
	}
}

void ACCharacter::SetRagdollEnabled(bool bIsEnabled)
{
	if (bIsEnabled)
	{
		//  将骨骼网格体从父组件分离（保持当前世界变换）
		GetMesh()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		// 开启物理模拟
		GetMesh()->SetSimulatePhysics(true);
		// 设置碰撞模式为仅物理交互
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	}
	else
	{
		// 关闭物理模拟
		GetMesh()->SetSimulatePhysics(false);
		// 禁用所有碰撞
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// 将骨骼网格体重新附加到根组件
		GetMesh()->AttachToComponent(GetRootComponent(),FAttachmentTransformRules::KeepRelativeTransform);
		// 恢复骨骼网格体的初始相对变换
		GetMesh()->SetRelativeTransform(MeshRelativeTransform);
	}
}

void ACCharacter::StartDeathSequence()
{
	OnDeath();
	// 当死亡的时候,立即停下能力
	if (CAbilitySystemComponent)
	{
		CAbilitySystemComponent->CancelAbilities();
	}
	// 播放死亡动画
	PlayDeathAnimation();
	// 把头上的血条隐藏
	SetStatusGaugeEnabled(false);
	// 禁用角色的移动
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	// 禁用胶囊的碰撞
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// 设置自身不被AI察觉
	SetAIPerceptionStimuliSourceEnabled(false);
}

void ACCharacter::Respawn()
{
	// 触发复活事件
	OnRespawn();

	// 设置自身被AI察觉
	SetAIPerceptionStimuliSourceEnabled(true);
	// 禁用布娃娃模拟
	SetRagdollEnabled(false);
	// 重新启用胶囊体碰撞
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	// 重置移动模式为移动
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	// 停止所有正在播放的动画蒙太奇
	GetMesh()->GetAnimInstance()->StopAllMontages(0.f);
	// 重新启用状态条（血条等UI）
	SetStatusGaugeEnabled(true);

	if (HasAuthority() && GetController())
	{
		// 获取到玩家的出生点
		TWeakObjectPtr<AActor> StartSpot = GetController()->StartSpot;
		// 如果出生点有效,复活的时候再出生点复活
		if (StartSpot.IsValid())
		{
			SetActorTransform(StartSpot->GetActorTransform());
		}
	}
	
	if (CAbilitySystemComponent)
	{
		// 应用所有的效果
		CAbilitySystemComponent->ApplyFullStatEffect();
	}
}

void ACCharacter::OnDeath()
{
}

void ACCharacter::OnRespawn()
{
}

void ACCharacter::SetAIPerceptionStimuliSourceEnabled(bool bIsEnabled)
{
	if (!PerceptionStimuliSourceComponent) return;

	if (bIsEnabled)
	{
		// 将当前Actor注册为AI视觉感知系统的有效检测目标 
		PerceptionStimuliSourceComponent->RegisterWithPerceptionSystem();
	}
	else
	{
		// 将当前Actor取消注册为AI视觉感知系统的有效检测目标 
		PerceptionStimuliSourceComponent->UnregisterFromPerceptionSystem();
	}
}
