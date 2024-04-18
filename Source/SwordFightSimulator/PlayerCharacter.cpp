// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#define ROLE_TO_STRING(Value) FindObject<UEnum>(ANY_PACKAGE, TEXT("ENetRole"), true)->GetNameStringByIndex((int32)Value)

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//const FString Values = FString::Printf(TEXT("Health : %\nLocal Role : %s\nRemote Role : %s"), HealthPoint, *ROLE_TO_STRING(GetLocalRole()), *ROLE_TO_STRING(GetRemoteRole()));
	//DrawDebugString(GetWorld(), GetActorLocation(), Values, nullptr, FColor::White, 0.0f, true);
	//UE_LOG(LogTemp, Warning, TEXT("%s attack mode called %d, owner %s"), *this->GetActorNameOrLabel(), bIsAttacking, *GetNetOwner()->GetName());

	const FString LocalRoleString = ROLE_TO_STRING(GetLocalRole());
	const FString RemoteRoleString = ROLE_TO_STRING(GetRemoteRole());
	const FString OwnerString = GetNetOwner() != nullptr ? GetNetOwner()->GetName() : TEXT("No Owner");
	const FString ConnectionString = GetNetConnection() != nullptr ? TEXT("Valid") : TEXT("In-valid");

	const FString Values = FString::Printf(
		TEXT("Local: %s\nRemote: %s\nOwner: %s\nConnection: %s"),
		*LocalRoleString,
		*RemoteRoleString,
		*OwnerString,
		*ConnectionString);

	FColor Color = GetNetConnection() != nullptr ? FColor::Green : FColor::Red;
	DrawDebugString(GetWorld(), GetActorLocation(), Values, nullptr, Color, 0.f, true);
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

void APlayerCharacter::StartAttackMode(const FInputActionValue& Value)
{
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
		if (UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			SubSystem->AddMappingContext(AttackMappingContext, 1);
			FViewport* Viewport = LocalPlayer->ViewportClient->Viewport;
			if (Viewport)
			{
				FVector2D ViewportSize;
				LocalPlayer->ViewportClient->GetViewportSize(ViewportSize);
				const int32 X = static_cast<int32>(ViewportSize.X * 0.5f);
				const int32 Y = static_cast<int32>(ViewportSize.Y * 0.5f);
				Viewport->SetMouse(X, Y);
			}
		}
	}
	ServerSetAttackMode(true);
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
	//UE_LOG(LogTemp, Warning, TEXT("Attack"));
	APlayerController* PlayerController = Cast<APlayerController>(GetController());

	FVector WorldLocation;
	FVector WorldDirection;
	if (PlayerController->DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
	{
		FVector TempRightHandLocation = GetMesh()->GetComponentTransform().InverseTransformPosition(WorldLocation + WorldDirection * 100.0f);
		ServerSetRightHandLocation(TempRightHandLocation);

		//RightHandLocation *= FVector(1.0f, 1.0f, 1.0f);
		//DrawDebugSphere(GetWorld(), CursorLocation, 1.0f, 20, FColor::Red, true);
		//UE_LOG(LogTemp, Warning, TEXT("%f %f %f"), CursorLocation.X, CursorLocation.Y, CursorLocation.Z);
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

			PlayerController->PlayerCameraManager->ViewPitchMin = -30.0f;
			PlayerController->PlayerCameraManager->ViewPitchMax = 30.0f;
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
