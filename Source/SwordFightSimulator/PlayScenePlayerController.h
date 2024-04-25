// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerCharacter.h"
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

protected:
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable)
	void ServerSpawnPlayer();

};
