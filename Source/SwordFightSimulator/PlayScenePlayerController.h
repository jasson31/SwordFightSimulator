// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerCharacter.h"
#include "HostWaitingWidget.h"
#include "GameEndWidget.h"
#include "PlayScenePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class SWORDFIGHTSIMULATOR_API APlayScenePlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Play Scene Player Controller")
	TSubclassOf<APlayerCharacter> PlayerCharacterBlueprint;

	UPROPERTY(EditDefaultsOnly, Category = "Play Scene Game Mode")
	TSubclassOf<UUserWidget> HostWaitingWidgetBlueprint;
	UHostWaitingWidget* HostWaitingWidget;
	UPROPERTY(EditDefaultsOnly, Category = "Play Scene Game Mode")
	TSubclassOf<UUserWidget> GameEndWidgetBlueprint;
	UGameEndWidget* GameEndWidget;
	UPROPERTY(Replicated)
	bool bHasPlayerWon = false;

public:
	UFUNCTION(Server, Reliable)
	void ServerSpawnPlayer();
	UFUNCTION(Client, Reliable)
	void ClientCreateHostWaitingWidget();
	UFUNCTION(Client, Reliable)
	void ClientRemoveHostWaitingWidget() const;
	UFUNCTION(Server, Reliable)
	void ServerSetPlayerGameEnd(bool Value);
	UFUNCTION(Client, Reliable)
	void ClientCreateGameEndWidget();
	FString GetbHasPlayerWonText() const { return bHasPlayerWon ? "You win!" : "You lose..."; }
};
