// Fill out your copyright notice in the Description page of Project Settings.


#include "PlaySceneGameMode.h"
#include <Kismet/GameplayStatics.h>
#include <GameFramework/PlayerStart.h>

void APlaySceneGameMode::BeginPlay()
{
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStartTransforms);
}

AActor* APlaySceneGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	return nullptr;
}
