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

void APlayerCharacter::ApplyParried()
{
	ParryDuration -= ParryDelay;

	float BounceRatio = FMath::Pow(ParryDuration / CurrMaxParryDuration, 2) * 3;
	float NextAimPitch = FMath::Clamp(AttackPitch - ParryPitchDiff * BounceRatio, AttackCenterPitch + AttackPitchClamp.X, AttackCenterPitch + AttackPitchClamp.Y);
	float NextAimYaw = FMath::Clamp(AttackYaw - ParryYawDiff * BounceRatio, AttackCenterYaw + AttackYawClamp.X, AttackCenterYaw + AttackYawClamp.Y);


	FVector AimVector = FRotator(NextAimPitch, NextAimYaw, 0.0f).Vector();
	FVector AimLocation = UKismetMathLibrary::ComposeTransforms(GetMesh()->GetSocketTransform("CameraSocket", ERelativeTransformSpace::RTS_World), FTransform(AimVector * 200.0f)).GetLocation();

	FVector PrevAimVector = FRotator(AttackPitch, AttackYaw, 0.0f).Vector();
	FVector PrevAimLocation = UKismetMathLibrary::ComposeTransforms(GetMesh()->GetSocketTransform("CameraSocket", ERelativeTransformSpace::RTS_World), FTransform(PrevAimVector * 200.0f)).GetLocation();

	FVector LocalRightHandLocation = GetMesh()->GetComponentTransform().InverseTransformPosition(AimLocation);

	ServerSetRightHandLocation(LocalRightHandLocation);
	AttackPitch = NextAimPitch;
	AttackYaw = NextAimYaw;

	if (ParryDuration < 0)
	{
		bIsParried = false;
		GetWorldTimerManager().ClearTimer(ParriedTimer);
	}
}

void APlayerCharacter::SetRightHandLocation() 
{
	if (!bIsParried)
	{
		float NextAimPitch = FMath::Clamp(AttackPitch + AttackPitchDiff, AttackCenterPitch + AttackPitchClamp.X, AttackCenterPitch + AttackPitchClamp.Y);
		float NextAimYaw = FMath::Clamp(AttackYaw + AttackYawDiff, AttackCenterYaw + AttackYawClamp.X, AttackCenterYaw + AttackYawClamp.Y);

		FVector AimVector = FRotator(NextAimPitch, NextAimYaw, 0.0f).Vector();
		FVector AimLocation = UKismetMathLibrary::ComposeTransforms(GetMesh()->GetSocketTransform("CameraSocket", ERelativeTransformSpace::RTS_World), FTransform(AimVector * 200.0f)).GetLocation();

		FVector PrevAimVector = FRotator(AttackPitch, AttackYaw, 0.0f).Vector();
		FVector PrevAimLocation = UKismetMathLibrary::ComposeTransforms(GetMesh()->GetSocketTransform("CameraSocket", ERelativeTransformSpace::RTS_World), FTransform(PrevAimVector * 200.0f)).GetLocation();

		FVector LocalRightHandLocation = GetMesh()->GetComponentTransform().InverseTransformPosition(AimLocation);
		FVector AimMoveDirection = AimLocation - PrevAimLocation;
		AimMoveDirection.Normalize();

		if (!MySword->CheckSwordBlocked(AimMoveDirection))
		{
			ServerSetRightHandLocation(LocalRightHandLocation);
			AttackPitch = NextAimPitch;
			AttackYaw = NextAimYaw;
		}
		else
		{
			float ParryStrength = FMath::Sqrt(FMath::Pow(AttackPitchDiff, 2) + FMath::Pow(AttackYawDiff, 2));
			CurrMaxParryDuration = ParryDuration = FMath::Clamp(ParryStrength, MinParryDuration, MaxParryDuration);
			ParryPitchDiff = AttackPitchDiff * CurrMaxParryDuration / MaxParryDuration * ParryStrengthRatio;
			ParryYawDiff = AttackYawDiff * CurrMaxParryDuration / MaxParryDuration * ParryStrengthRatio;
			bIsParried = true;

			GetWorldTimerManager().SetTimer(ParriedTimer, this, &APlayerCharacter::ApplyParried, ParryDelay, true, 0.0f);
		}
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
	AttackPitch = AttackCenterPitch = CurrentHeadRotator.Pitch;
	AttackYaw = AttackCenterYaw = CurrentHeadRotator.Yaw;
	SetRightHandLocation();
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
	ServerSetAttackMode(false);
}

void APlayerCharacter::Attack(const FInputActionValue& Value)
{
	const FVector2D InputValue = Value.Get<FVector2D>();
	AttackYawDiff = FMath::Clamp(InputValue.X, AttackInputYawDiffClamp.X, AttackInputYawDiffClamp.Y);
	AttackPitchDiff = FMath::Clamp(InputValue.Y, AttackInputPitchDiffClamp.X, AttackInputPitchDiffClamp.Y);
	SetRightHandLocation();
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
