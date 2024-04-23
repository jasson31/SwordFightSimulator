// Fill out your copyright notice in the Description page of Project Settings.


#include "Sword.h"
#include "Damagable.h"
#include "PlayerCharacter.h"

// Sets default values
ASword::ASword()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SwordMesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh Component");
	RootComponent = SwordMesh;

	bReplicates = true;
}

bool ASword::CheckSwordBlocked(FVector CheckDirection)
{
	FHitResult HitResult;

	FVector TopBound = SwordMesh->GetSocketLocation("Top Bound");
	FVector BottomBackRightBound = SwordMesh->GetSocketLocation("Bottom Right Bound");
	FVector BottomFrontLeftBound = SwordMesh->GetSocketLocation("Bottom Left Bound");
	FVector BottomCenterBound = (BottomBackRightBound + BottomFrontLeftBound) / 2;

	FVector Start = (TopBound + BottomCenterBound) / 2;
	FVector End = Start + CheckDirection * 1.0f;
	float HalfWidth = FMath::Abs(BottomBackRightBound.X - BottomFrontLeftBound.X) / 2;
	float HalfDepth = FMath::Abs(BottomBackRightBound.Y - BottomFrontLeftBound.Y) / 2;
	float HalfHeight = FVector::Dist(TopBound, BottomCenterBound) / 2;
	FVector HalfExtent(HalfWidth, HalfDepth, HalfHeight);

	TArray<AActor*> IgnoredActors = { this, GetOwner() };
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActors(IgnoredActors);
	bool IsHit = GetWorld()->SweepSingleByChannel(HitResult, Start, End, GetActorRotation().Quaternion(), ECollisionChannel::ECC_GameTraceChannel3, FCollisionShape::MakeBox(HalfExtent), QueryParams);
	//DrawDebugBox(GetWorld(), Start, HalfExtent, GetActorRotation().Quaternion(), FColor::Yellow, false, 0.0f);
	//DrawDebugBox(GetWorld(), End, HalfExtent, GetActorRotation().Quaternion(), FColor::Red, false, 0.0f);
	return IsHit;
}

// Called when the game starts or when spawned
void ASword::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		SwordMesh->OnComponentBeginOverlap.AddDynamic(this, &ASword::OnOverlapBegin);
		SwordMesh->OnComponentEndOverlap.AddDynamic(this, &ASword::OnOverlapEnd);
	}
}

void ASword::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Owner != OtherActor)
	{
		if (IDamagable* HitDamagable = Cast<IDamagable>(OtherActor))
		{
			APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(Owner);
			if (PlayerCharacter == nullptr || PlayerCharacter->GetbIsAttacking())
			{
				Cast<APlayerCharacter>(OtherActor)->ServerProcessDamage(OtherActor, Damage);
			}
		}
	}
}

void ASword::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	IDamagable* HitDamagable = Cast<IDamagable>(OtherActor);
	if (HitDamagable != nullptr && Owner != OtherActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s hit end %s"), *this->GetActorNameOrLabel(), *OtherActor->GetActorNameOrLabel());
	}
}
