// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameEndWidget.generated.h"

/**
 * 
 */
UCLASS()
class SWORDFIGHTSIMULATOR_API UGameEndWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	bool bHasWon = false;

public:
	void SetbHasWon(bool Value) { bHasWon = Value; }
	UFUNCTION(BlueprintCallable)
	FString GetGameEndText() const { return bHasWon ? "You Win!" : "You Lose..."; }

};
