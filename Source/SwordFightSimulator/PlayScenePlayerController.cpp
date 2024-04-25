// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayScenePlayerController.h"
#include "PlaySceneGameMode.h"
#include <Kismet/GameplayStatics.h>

void APlayScenePlayerController::BeginPlay()
{
	if (IsLocalPlayerController())
	{
		ServerSpawnPlayer();
	}
}

void APlayScenePlayerController::ServerSpawnPlayer_Implementation()
{
	if (GetPawn() != nullptr)
	{
		GetPawn()->Destroy();
	}
	
	if (APlaySceneGameMode* GameMode = Cast<APlaySceneGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		AActor* PlayerStart = GameMode->GetPlayerStartPosition(this);
		FTransform PlayerStartTransform = PlayerStart->GetActorTransform();
		APawn* NewPawn = GetWorld()->SpawnActor<APlayerCharacter>(PlayerCharacterBlueprint, PlayerStartTransform);
		Possess(NewPawn);
	}
}
