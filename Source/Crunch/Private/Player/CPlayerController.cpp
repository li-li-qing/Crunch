// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "CPlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Widgets/GameplayWidget.h"
void ACPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	CPlayerCharacter = Cast<ACPlayerCharacter>(InPawn);
	if (CPlayerCharacter)
	{
		CPlayerCharacter->Server_SideInit();
		CPlayerCharacter->SetGenericTeamId(TeamId);

	}
}

void ACPlayerController::AcknowledgePossession(APawn* InPawn)
{
	Super::AcknowledgePossession(InPawn);

	CPlayerCharacter = Cast<ACPlayerCharacter>(InPawn);
	if (CPlayerCharacter)
	{
		CPlayerCharacter->Client_SideInit();
		SpawnGameplayWidget();
	}
}

void ACPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACPlayerController,TeamId);
}

void ACPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// 获得增强输入子系统
	UEnhancedInputLocalPlayerSubsystem* InputSubsystem = GetLocalPlayer()->GetSubsystem<
		UEnhancedInputLocalPlayerSubsystem>();
	if (InputSubsystem)
	{
		// 移除映射上下文
		InputSubsystem->RemoveMappingContext(UIInputMapping);
		// 添加映射上下文，优先级为0
		InputSubsystem->AddMappingContext(UIInputMapping, 1);
	}
	UEnhancedInputComponent* EnhancedInputComp = Cast<UEnhancedInputComponent>(InputComponent);
	if (EnhancedInputComp)
	{
		EnhancedInputComp->BindAction(ShopToggleInputAction,ETriggerEvent::Triggered,this,&ACPlayerController::ToggleShop);
	}
}

void ACPlayerController::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	
	TeamId = NewTeamID;
}

FGenericTeamId ACPlayerController::GetGenericTeamId() const
{
	return TeamId;
}

void ACPlayerController::SpawnGameplayWidget()
{
	// 如果当前不是本地玩家就直接退出
	if (!IsLocalPlayerController()) return;
	
	// 创建这个Widget然后添加到视口
	GameplayWidget = CreateWidget<UGameplayWidget>(this,GameplayWidgetClass);
	if (GameplayWidget)
	{
		GameplayWidget->AddToViewport();
		// 把拥有的技能传给UI
		GameplayWidget->ConfigureAbilities(CPlayerCharacter->GetAbilities());
	}
}

void ACPlayerController::ToggleShop()
{
	GameplayWidget->ToggleShop();
}
