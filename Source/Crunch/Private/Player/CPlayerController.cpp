// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "CPlayerCharacter.h"
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
	}
}
