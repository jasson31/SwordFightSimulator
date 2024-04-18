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

	//CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>("Capsule Component");
	//CapsuleComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);

	bReplicates = true;
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

		UE_LOG(LogTemp, Warning, TEXT("%s hit %s %s, %s"), *this->GetActorNameOrLabel(), *OtherActor->GetActorNameOrLabel(), *SweepResult.ImpactPoint.ToString(), *SweepResult.ImpactNormal.ToString());
		DrawDebugLine(GetWorld(), SweepResult.ImpactPoint, SweepResult.ImpactPoint + SweepResult.ImpactNormal * 5.0f, FColor::Red, false, 5.0f);
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
