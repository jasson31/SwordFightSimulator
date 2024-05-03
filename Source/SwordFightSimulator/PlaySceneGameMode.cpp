// Fill out your copyright notice in the Description page of Project Settings.


#include "PlaySceneGameMode.h"
#include <Kismet/GameplayStatics.h>
#include <GameFramework/PlayerStart.h>
#include "Engine.h"
#include "PlayScenePlayerController.h"

void APlaySceneGameMode::PostLogin(APlayerController* NewPlayer)
{
	if (UGameplayStatics::HasOption(OptionsString, FString("listen")) && this->GetNumPlayers() == 1)
	{
		if (APlayScenePlayerController* PlayerController = Cast< APlayScenePlayerController>(NewPlayer))
		{
			PlayerController->ClientCreateHostWaitingWidget();
		}
	}
	else
	{
		for (APlayScenePlayerController* Controller : TActorRange<APlayScenePlayerController>(GetWorld()))
		{
			Controller->ServerSpawnPlayer();
			Controller->ClientRemoveHostWaitingWidget();
		}
	}
}

AActor* APlaySceneGameMode::GetPlayerStartPosition(AController* Player)
{
	if (PlayerStartTransforms.IsEmpty())
	{
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStartTransforms);
	}
	return CurrentPlayerCount < PlayerStartTransforms.Num() ? PlayerStartTransforms[CurrentPlayerCount++] : nullptr;
}
