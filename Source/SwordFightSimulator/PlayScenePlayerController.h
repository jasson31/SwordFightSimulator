// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerCharacter.h"
#include "HostWaitingWidget.h"
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
	UPROPERTY()
	UHostWaitingWidget* HostWaitingWidget;

public:
	UFUNCTION(Server, Reliable)
	void ServerSpawnPlayer();
	void CreateHostWaitingWidget();
	void RemoveHostWaitingWidget() const;
};
