// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include <GameFramework/SpringArmComponent.h>
#include <Camera/CameraComponent.h>
#include "Damagable.h"
#include "Net/UnrealNetwork.h"
#include "Sword.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class SWORDFIGHTSIMULATOR_API APlayerCharacter : public ACharacter, public IDamagable
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = Input)
	UInputMappingContext* DefaultMappingContext;
	UPROPERTY(EditDefaultsOnly, Category = Input)
	UInputMappingContext* AttackMappingContext;
	UPROPERTY(EditDefaultsOnly, Category = Input)
	UInputAction* MoveAction;
	UPROPERTY(EditDefaultsOnly, Category = Input)
	UInputAction* LookAction;
	UPROPERTY(EditDefaultsOnly, Category = Input)
	UInputAction* AttackModeAction;
	UPROPERTY(EditDefaultsOnly, Category = Input)
	UInputAction* AttackAction;

	UFUNCTION(Server, Reliable)
	void ServerSetAttackMode(bool AttackMode);
	UFUNCTION(Server, Reliable)
	void ServerSetRightHandLocation(FVector NewHandLocation);
	void SetRightHandLocation();

	/*UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = PlayerCamera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Camera")
	UCameraComponent* Camera;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<ASword> SwordBlueprint;


	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void StartAttackMode(const FInputActionValue& Value);
	void StopAttackMode(const FInputActionValue& Value);
	void Attack(const FInputActionValue& Value);

	void SetHealthPoint(float Value) { HealthPoint = Value; }
	void AdjustHealthPoint(float Value) { SetHealthPoint(FMath::Clamp(HealthPoint + Value, 0.0f, MaxHealthPoint)); }
	
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Player Character")
	float HealthPoint = 0.0f;
	float MaxHealthPoint = 100.0f;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Player Character")
	FVector RightHandLocation;
	//FVector PrevAimLocation;
	FVector PrevRightHandDirection;
	FVector LeftHandLocation;
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Player Character")
	ASword* MySword;
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Player Character")
	bool bIsAttacking = false;

	float AttackPitch = 0.0f;
	float AttackYaw = 0.0f;
	float AttackPitchDiff;
	float AttackYawDiff;
	float AttackCenterPitch;
	float AttackCenterYaw;
	FVector2f AttackInputYawDiffClamp = FVector2f(-4.0f, 4.0f);
	FVector2f AttackInputPitchDiffClamp = FVector2f(-4.0f, 4.0f);
	FVector2f AttackYawClamp = FVector2f(-30.0f, 30.0f);
	FVector2f AttackPitchClamp = FVector2f(-30.0f, 30.0f);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void OnDamaged(float Damage) override;

	UFUNCTION(BlueprintCallable)
	void GetHealthInformation(float& OutCurrentHealthPoint, float& OutMaxHealthPoint) { OutCurrentHealthPoint = HealthPoint; OutMaxHealthPoint = MaxHealthPoint; }
	UFUNCTION(BlueprintCallable)
	FVector GetRightHandLocation() const { return RightHandLocation; }
	UFUNCTION(BlueprintCallable)
	FVector GetLeftHandLocation();
	UFUNCTION(BlueprintCallable)
	bool GetbIsAttacking() const { return bIsAttacking; }
	UFUNCTION(Server, Reliable)
	void ServerProcessDamage(AActor* Actor, float Damage);
};
