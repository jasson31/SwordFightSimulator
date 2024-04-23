// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include <Kismet/KismetMathLibrary.h>

#define ROLE_TO_STRING(Value) FindObject<UEnum>(ANY_PACKAGE, TEXT("ENetRole"), true)->GetNameStringByIndex((int32)Value)

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	//CameraBoom->SetupAttachment(RootComponent);
	//CameraBoom->TargetArmLength = 300.0f;
	//CameraBoom->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
	//Camera->SetupAttachment(CameraBoom);
	Camera->SetupAttachment(GetMesh(), "CameraSocket");
	Camera->bUsePawnControlRotation = true;
	//bUseControllerRotationPitch = false;
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		FActorSpawnParameters SpawnParameters = FActorSpawnParameters();
		SpawnParameters.Owner = this;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		MySword = GetWorld()->SpawnActor<ASword>(SwordBlueprint, SpawnParameters);

		if (MySword == nullptr)
		{
			return;
		}

		MySword->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "Grip Point Socket");

		SetHealthPoint(MaxHealthPoint);
	}
}

void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayerCharacter, HealthPoint);
	DOREPLIFETIME(APlayerCharacter, RightHandLocation);
	DOREPLIFETIME(APlayerCharacter, MySword);
	DOREPLIFETIME(APlayerCharacter, bIsAttacking);
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D InputValue = Value.Get<FVector2D>();
	if (InputValue.X != 0.0f)
	{
		AddMovementInput(GetActorRightVector(), InputValue.X);
	}
	if (InputValue.Y != 0.0f)
	{
		AddMovementInput(GetActorForwardVector(), InputValue.Y);
	}
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D InputValue = Value.Get<FVector2D>();
	if (InputValue.X != 0.0f)
	{
		AddControllerYawInput(InputValue.X);
	}
	if (InputValue.Y != 0.0f)
	{
		AddControllerPitchInput(-InputValue.Y);
	}
}

void APlayerCharacter::ApplyParried(FVector2f ParryAimDiff, float CurrMaxParryDuration)
{
	ParryDuration -= ParryApplyDelay;
	float BounceRatio = FMath::Pow(ParryDuration / CurrMaxParryDuration, 2) * 3;
	SetRightHandLocation(-ParryAimDiff * BounceRatio, false);
	if (ParryDuration < 0)
	{
		bIsParried = false;
		GetWorldTimerManager().ClearTimer(ParriedTimerHandle);
		ParriedTimerDelegate.Unbind();
	}
}

void APlayerCharacter::SetRightHandLocation(FVector2f AttackAimDiff, bool CheckSwordMovable)
{
	float NextAimPitch = FMath::Clamp(CurrAttackAim.X + AttackAimDiff.X, AttackAimCenter.X + AttackPitchClamp.X, AttackAimCenter.X + AttackPitchClamp.Y);
	float NextAimYaw = FMath::Clamp(CurrAttackAim.Y + AttackAimDiff.Y, AttackAimCenter.Y + AttackYawClamp.X, AttackAimCenter.Y + AttackYawClamp.Y);

	FVector AimVector = FRotator(NextAimPitch, NextAimYaw, 0.0f).Vector();
	FVector AimLocation = UKismetMathLibrary::ComposeTransforms(GetMesh()->GetSocketTransform("CameraSocket", ERelativeTransformSpace::RTS_World), FTransform(AimVector * 200.0f)).GetLocation();

	FVector PrevAimVector = FRotator(CurrAttackAim.X, CurrAttackAim.Y, 0.0f).Vector();
	FVector PrevAimLocation = UKismetMathLibrary::ComposeTransforms(GetMesh()->GetSocketTransform("CameraSocket", ERelativeTransformSpace::RTS_World), FTransform(PrevAimVector * 200.0f)).GetLocation();

	FVector TempRightHandLocation = GetMesh()->GetComponentTransform().InverseTransformPosition(AimLocation);

	FVector AimMoveDirection = AimLocation - PrevAimLocation;
	AimMoveDirection.Normalize();
	if (!(CheckSwordMovable && MySword->CheckSwordBlocked(AimMoveDirection)))
	{
		ServerSetRightHandLocation(TempRightHandLocation);
		CurrAttackAim = FVector2f(NextAimPitch, NextAimYaw);
	}
	else
	{
		float ParryStrength = AttackAimDiff.Length();
		ParryDuration = FMath::Clamp(ParryStrength, MinParryDuration, MaxParryDuration);
		FVector2f ParryAimDiff = AttackAimDiff * ParryDuration / MaxParryDuration * ParryStrengthRatio;
		bIsParried = true;
		ParriedTimerDelegate.BindUFunction(this, FName("ApplyParried"), ParryAimDiff, ParryDuration);
		GetWorldTimerManager().SetTimer(ParriedTimerHandle, ParriedTimerDelegate, ParryApplyDelay, true, 0.0f);
	}
}

void APlayerCharacter::StartAttackMode(const FInputActionValue& Value)
{
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
		if (UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			SubSystem->AddMappingContext(AttackMappingContext, 1);
		}
	}
	ServerSetAttackMode(true);

	FRotator CurrentHeadRotator = GetMesh()->GetSocketTransform("CameraSocket", ERelativeTransformSpace::RTS_World).Rotator();
	CurrAttackAim = AttackAimCenter = FVector2f(CurrentHeadRotator.Pitch, CurrentHeadRotator.Yaw);
	SetRightHandLocation(FVector2f(0.0f, 0.0f));
}

void APlayerCharacter::Attack(const FInputActionValue& Value)
{
	if (!bIsParried)
	{
		const FVector2D InputValue = Value.Get<FVector2D>();
		FVector2f AttackAimDiff(FMath::Clamp(InputValue.Y, AttackInputPitchDiffClamp.X, AttackInputPitchDiffClamp.Y), FMath::Clamp(InputValue.X, AttackInputYawDiffClamp.X, AttackInputYawDiffClamp.Y));
		SetRightHandLocation(AttackAimDiff);
	}
}

void APlayerCharacter::StopAttackMode(const FInputActionValue& Value)
{
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
		if (UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
		{
			SubSystem->RemoveMappingContext(AttackMappingContext);
		}
	}
	GetWorldTimerManager().SetTimer(EndAttackTimerHandle, this, &APlayerCharacter::EndAttack, 0.01f, true, 0.0f);
}

void APlayerCharacter::EndAttack()
{
	if (!bIsParried)
	{
		ServerSetAttackMode(false);
		GetWorldTimerManager().ClearTimer(EndAttackTimerHandle);
	}
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedPlayerInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
		{
			if (UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			{
				SubSystem->AddMappingContext(DefaultMappingContext, 0);
			}

			PlayerController->PlayerCameraManager->ViewPitchMin = 0.0f;
			PlayerController->PlayerCameraManager->ViewPitchMax = 0.0f;
		}

		EnhancedPlayerInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
		EnhancedPlayerInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
		EnhancedPlayerInputComponent->BindAction(AttackModeAction, ETriggerEvent::Started, this, &APlayerCharacter::StartAttackMode);
		EnhancedPlayerInputComponent->BindAction(AttackModeAction, ETriggerEvent::Completed, this, &APlayerCharacter::StopAttackMode);
		EnhancedPlayerInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Attack);
	}
}

void APlayerCharacter::OnDamaged(float Damage)
{
	UE_LOG(LogTemp, Warning, TEXT("Lost Health"));
	AdjustHealthPoint(-Damage);
}

FVector APlayerCharacter::GetLeftHandLocation()
{
	if (MySword != nullptr)
	{
		FVector SocketPosition = MySword->GetComponentByClass<UStaticMeshComponent>()->GetSocketTransform(FName("Left Hand Socket")).GetLocation();
		LeftHandLocation = GetMesh()->GetComponentTransform().InverseTransformPosition(SocketPosition);
	}
	return LeftHandLocation;
}

void APlayerCharacter::ServerSetAttackMode_Implementation(bool AttackMode)
{
	bIsAttacking = AttackMode;
}

void APlayerCharacter::ServerSetRightHandLocation_Implementation(FVector NewHandLocation)
{
	RightHandLocation = NewHandLocation;
}

void APlayerCharacter::ServerProcessDamage_Implementation(AActor* Actor, float Damage)
{
	Cast<IDamagable>(Actor)->OnDamaged(Damage);
}
