// Fill out your copyright notice in the Description page of Project Settings.


#include "Sword.h"
#include "Damagable.h"
#include "PlayerCharacter.h"

// Sets default values
ASword::ASword()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SwordMesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh Component");
	RootComponent = SwordMesh;

	//CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>("Capsule Component");
	//CapsuleComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);

	bReplicates = true;
}

bool ASword::CheckSwordMovable(FVector CheckDirection)
{
	FHitResult HitResult;
	FVector Start = GetActorLocation() + GetActorUpVector() * 54.0f;
	FVector End = Start + CheckDirection;
	float Height = 47.0f;
	float Radius = 3.0f;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(GetOwner());
	bool IsHit = GetWorld()->SweepSingleByChannel(HitResult, Start, End, GetActorRotation().Quaternion(), ECollisionChannel::ECC_GameTraceChannel3, FCollisionShape::MakeCapsule(Radius, Height), QueryParams);
	DrawDebugCapsule(GetWorld(), Start, Height, Radius, GetActorRotation().Quaternion(), FColor::Yellow, false, 0.0f);
	DrawDebugCapsule(GetWorld(), End, Height, Radius, GetActorRotation().Quaternion(), FColor::Red, false, 0.0f);

	if (IsHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s hit %s %s, %s"), *this->GetActorNameOrLabel(), *HitResult.GetActor()->GetActorNameOrLabel(), *HitResult.ImpactPoint.ToString(), *HitResult.ImpactNormal.ToString());
		DrawDebugLine(GetWorld(), HitResult.ImpactPoint, HitResult.ImpactPoint + HitResult.ImpactNormal * 5.0f, FColor::Red, false, 5.0f);
	}
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
		//SwordMesh->OnComponentHit.AddDynamic(this, &ASword::OnHit);
	}
}

void ASword::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Owner != OtherActor)
	{
		if (IDamagable* HitDamagable = Cast<IDamagable>(OtherActor))
		{
			//UE_LOG(LogTemp, Warning, TEXT("%s hit %s"), *this->GetActorNameOrLabel(), *OtherActor->GetActorNameOrLabel());
			Cast<APlayerCharacter>(OtherActor)->ServerProcessDamage(OtherActor, Damage);

		}


		//FHitResult HitResult;
		//FVector Start = GetActorLocation() + GetActorUpVector();
		//FVector End = GetActorLocation() + GetActorUpVector() * 100.0f;
		//float Height = 30.0f;
		//float Radius = 10.0f;
		//FCollisionQueryParams QueryParams;

		//QueryParams.AddIgnoredActor(GetOwner());
		//bool IsHit = GetWorld()->SweepSingleByChannel(HitResult, Start, End, GetActorRotation().Quaternion(), ECollisionChannel::ECC_GameTraceChannel3, FCollisionShape::MakeCapsule(Radius, Height), QueryParams);
		//DrawDebugCapsule(GetWorld(), Start, Height, Radius, GetActorRotation().Quaternion(), FColor::Yellow, false, 0.0f);
		//DrawDebugCapsule(GetWorld(), End, Height, Radius, GetActorRotation().Quaternion(), FColor::Red, false, 0.0f);

		//if (IsHit)
		//{
		//	UE_LOG(LogTemp, Warning, TEXT("%s hit %s %s, %s"), *this->GetActorNameOrLabel(), *HitResult.GetActor()->GetActorNameOrLabel(), *HitResult.ImpactPoint.ToString(), *HitResult.ImpactNormal.ToString());
		//	DrawDebugLine(GetWorld(), HitResult.ImpactPoint, HitResult.ImpactPoint + HitResult.ImpactNormal * 5.0f, FColor::Red, false, 5.0f);
		//}
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

void ASword::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

//void ASword::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
//{
//	if (Owner != OtherActor)
//	{
//		if (IDamagable* HitDamagable = Cast<IDamagable>(OtherActor))
//		{
//			UE_LOG(LogTemp, Warning, TEXT("%s hit %s"), *this->GetActorNameOrLabel(), *OtherActor->GetActorNameOrLabel());
//			Cast<APlayerCharacter>(OtherActor)->ServerProcessDamage(OtherActor, Damage);
//		}
//	}
//}
