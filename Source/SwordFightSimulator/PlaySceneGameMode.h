// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HostWaitingWidget.h"
#include "PlaySceneGameMode.generated.h"

/**
 * 
 */
UCLASS()
class SWORDFIGHTSIMULATOR_API APlaySceneGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:
	int32 CurrentPlayerCount = 0;
	TArray<AActor*> PlayerStartTransforms;
	UPROPERTY(EditDefaultsOnly, Category = "Play Scene Game Mode")
	TSubclassOf<UHostWaitingWidget> HostWaitingWidgetBlueprint;
	UPROPERTY()
	UHostWaitingWidget* HostWaitingWidget;

protected:
	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;

public:

	AActor* GetPlayerStartPosition(AController* Player);
};
