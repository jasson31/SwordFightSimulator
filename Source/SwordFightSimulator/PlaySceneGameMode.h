// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PlaySceneGameMode.generated.h"

/**
 * 
 */
UCLASS()
class SWORDFIGHTSIMULATOR_API APlaySceneGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	short CurrentSpawnPoint;

protected:

	int32 MaxPlayerNumber;

	TArray<AActor*> PlayerStartTransforms;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
};
