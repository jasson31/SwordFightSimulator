// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayScenePlayerController.h"
#include "PlaySceneGameMode.h"
#include <Kismet/GameplayStatics.h>

FString APlayScenePlayerController::GetServerIPAddress()
{
	if (NetConnection)
	{
		return *NetConnection->URL.Host;
	}
	else
	{
		return "NetConnection is null";
	}
}

void APlayScenePlayerController::BeginPlay()
{
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
		this->Possess(NewPawn);
	}
	UE_LOG(LogTemp, Warning, TEXT("%s"), *GetServerIPAddress());
}
