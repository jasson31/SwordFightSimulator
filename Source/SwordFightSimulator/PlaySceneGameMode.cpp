// Fill out your copyright notice in the Description page of Project Settings.


#include "PlaySceneGameMode.h"
#include <Kismet/GameplayStatics.h>
#include <GameFramework/PlayerStart.h>
#include "Engine.h"


void APlaySceneGameMode::BeginPlay()
{
	CurrentPlayerCount = 0;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStartTransforms);
}

AActor* APlaySceneGameMode::GetPlayerStartPosition(AController* Player)
{
	return CurrentPlayerCount < PlayerStartTransforms.Num() ? PlayerStartTransforms[CurrentPlayerCount++] : nullptr;
}
