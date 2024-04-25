// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HostWaitingWidget.generated.h"

/**
 * 
 */
UCLASS()
class SWORDFIGHTSIMULATOR_API UHostWaitingWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintCallable)
	FString GetServerIPAddress();
	
};
