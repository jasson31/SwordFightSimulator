// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Components/CapsuleComponent.h>
#include "Sword.generated.h"

UCLASS()
class SWORDFIGHTSIMULATOR_API ASword : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Sword")
	UStaticMeshComponent* SwordMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Sword")
	float Damage = 10.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Sword")
	float DamageSwingPowerThreshold = 2.5f;

public:
	// Sets default values for this actor's properties
	ASword();
	bool CheckSwordBlocked(FVector CheckDirection);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
