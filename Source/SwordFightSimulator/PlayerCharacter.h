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

protected:
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

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = PlayerCamera, meta = (AllowPrivateAccess = "true"))
	//USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Camera")
	UCameraComponent* Camera;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<ASword> SwordBlueprint;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Player Character")
	float HealthPoint = 0.0f;
	float MaxHealthPoint = 100.0f;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Player Character")
	FVector RightHandLocation;
	FVector PrevRightHandDirection;
	FVector LeftHandLocation;
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Player Character")
	ASword* MySword;
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Player Character")
	bool bIsAttacking = false;
	FTimerHandle EndAttackTimerHandle;

	FVector2f CurrAttackAim;
	UPROPERTY(EditDefaultsOnly, Category = "Player Character")
	FVector2f AttackInputYawDiffClamp = FVector2f(-4.0f, 4.0f);
	UPROPERTY(EditDefaultsOnly, Category = "Player Character")
	FVector2f AttackInputPitchDiffClamp = FVector2f(-4.0f, 4.0f);
	UPROPERTY(EditDefaultsOnly, Category = "Player Character")
	FVector2f AttackYawClamp = FVector2f(-30.0f, 30.0f);
	UPROPERTY(EditDefaultsOnly, Category = "Player Character")
	FVector2f AttackPitchClamp = FVector2f(-30.0f, 30.0f);

	bool bIsParried = false;
	FTimerHandle ParriedTimerHandle;
	FTimerDelegate ParriedTimerDelegate;
	float ParryDuration;
	UPROPERTY(EditDefaultsOnly, Category = "Player Character")
	float MinParryDuration = 0.01f;
	UPROPERTY(EditDefaultsOnly, Category = "Player Character")
	float MaxParryDuration = 0.3f;
	UPROPERTY(EditDefaultsOnly, Category = "Player Character")
	float ParryApplyDelay = 0.01f;
	UPROPERTY(EditDefaultsOnly, Category = "Player Character")
	float ParryStrengthRatio = 0.3f;
	APlayerCharacter* EnemyPlayerCharacter;
	float LookInterpolationRatio = 0.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Player Character")
	float LookInterpolationSpeed = 5.0f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(Server, Reliable)
	void ServerSetAttackMode(bool AttackMode);
	UFUNCTION(Server, Reliable)
	void ServerSetRightHandLocation(FVector NewHandLocation);
	void SetRightHandLocation(FVector2f AttackAimDiff, bool CheckSwordMovable = true);

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	void StartAttackMode(const FInputActionValue& Value);
	void StopAttackMode(const FInputActionValue& Value);
	void Attack(const FInputActionValue& Value);
	UFUNCTION()
	void EndAttack();
	UFUNCTION()
	void ApplyParried(FVector2f ParryAimDiff, float CurrMaxParryDuration);
	void LookOpponent(float DeltaTime);

	void SetHealthPoint(float Value) { HealthPoint = Value; }
	void AdjustHealthPoint(float Value) { SetHealthPoint(FMath::Clamp(HealthPoint + Value, 0.0f, MaxHealthPoint)); }
	void Death(AActor* Attacker);

public:
	// Sets default values for this character's properties
	APlayerCharacter();

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void OnDamaged(AActor* Attacker, float Damage) override;

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
