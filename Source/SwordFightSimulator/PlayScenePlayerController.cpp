// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayScenePlayerController.h"
#include "PlaySceneGameMode.h"
#include <Kismet/GameplayStatics.h>

void APlayScenePlayerController::CreateHostWaitingWidget()
{
	if (UKismetSystemLibrary::IsServer(GetWorld()) && IsLocalPlayerController())
	{
		HostWaitingWidget = Cast<UHostWaitingWidget>(CreateWidget(this, HostWaitingWidgetBlueprint));
		HostWaitingWidget->AddToViewport();
	}
}

void APlayScenePlayerController::RemoveHostWaitingWidget() const
{
	if (IsLocalPlayerController())
	{
		HostWaitingWidget->RemoveFromParent();
		HostWaitingWidget->Destruct();
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
		this->Possess(NewPawn);
	}
}
