// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayScenePlayerController.h"
#include "PlaySceneGameMode.h"
#include <Kismet/GameplayStatics.h>


void APlayScenePlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayScenePlayerController, bHasPlayerWon);
}

void APlayScenePlayerController::ClientCreateHostWaitingWidget_Implementation()
{
	HostWaitingWidget = Cast<UHostWaitingWidget>(CreateWidget(this, HostWaitingWidgetBlueprint));
	HostWaitingWidget->AddToViewport();
}

void APlayScenePlayerController::ClientRemoveHostWaitingWidget_Implementation() const
{
	if (HostWaitingWidget != nullptr)
	{
		HostWaitingWidget->RemoveFromParent();
		HostWaitingWidget->Destruct();
	}
}

void APlayScenePlayerController::ServerSetPlayerGameEnd_Implementation(bool Value)
{
	bHasPlayerWon = Value;
	ClientCreateGameEndWidget();
}

void APlayScenePlayerController::ClientCreateGameEndWidget_Implementation()
{
	GameEndWidget = Cast<UGameEndWidget>(CreateWidget(this, GameEndWidgetBlueprint));
	GameEndWidget->AddToViewport();
	GameEndWidget->SetbHasWon(bHasPlayerWon);
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
