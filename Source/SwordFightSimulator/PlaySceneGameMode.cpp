// Fill out your copyright notice in the Description page of Project Settings.


#include "PlaySceneGameMode.h"
#include <Kismet/GameplayStatics.h>
#include <GameFramework/PlayerStart.h>
#include "Engine.h"
#include "PlayScenePlayerController.h"

void APlaySceneGameMode::BeginPlay()
{
	CurrentPlayerCount = 0;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStartTransforms);
}

void APlaySceneGameMode::PostLogin(APlayerController* NewPlayer)
{
	if (this->GetNumPlayers() == 2)
	{
		for (APlayScenePlayerController* Controller : TActorRange<APlayScenePlayerController>(GetWorld()))
		{
			Controller->ServerSpawnPlayer();
		}
		//HostWaitingWidget->RemoveFromParent();
		//HostWaitingWidget->Destruct();
	}
	else
	{
		//HostWaitingWidget = CreateWidget<UHostWaitingWidget>(this, HostWaitingWidgetBlueprint);
		//HostWaitingWidget->AddToViewport();
	}
}

AActor* APlaySceneGameMode::GetPlayerStartPosition(AController* Player)
{
	return CurrentPlayerCount < PlayerStartTransforms.Num() ? PlayerStartTransforms[CurrentPlayerCount++] : nullptr;
}
