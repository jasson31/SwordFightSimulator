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
	switch (this->GetNumPlayers())
	{
	case 1:
		if (APlayScenePlayerController* PlayerController = Cast< APlayScenePlayerController>(NewPlayer))
		{
			PlayerController->CreateHostWaitingWidget();
		}
		break;
	case 2:
		for (APlayScenePlayerController* Controller : TActorRange<APlayScenePlayerController>(GetWorld()))
		{
			Controller->ServerSpawnPlayer();
			Controller->RemoveHostWaitingWidget();
		}
		break;
	default:
		break;
	}
}

AActor* APlaySceneGameMode::GetPlayerStartPosition(AController* Player)
{
	return CurrentPlayerCount < PlayerStartTransforms.Num() ? PlayerStartTransforms[CurrentPlayerCount++] : nullptr;
}
