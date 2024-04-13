// Fill out your copyright notice in the Description page of Project Settings.


#include "Sword.h"
#include "Damagable.h"

// Sets default values
ASword::ASword()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SwordMesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh Component");
	RootComponent = SwordMesh;

	//CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>("Capsule Component");
	//CapsuleComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
}

// Called when the game starts or when spawned
void ASword::BeginPlay()
{
	Super::BeginPlay();

	SwordMesh->OnComponentBeginOverlap.AddDynamic(this, &ASword::OnOverlapBegin);
	SwordMesh->OnComponentEndOverlap.AddDynamic(this, &ASword::OnOverlapEnd);
}

void ASword::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IDamagable* HitDamagable = Cast<IDamagable>(OtherActor);
	if (HitDamagable != nullptr && Owner != OtherActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit, %s"), *OtherActor->GetActorNameOrLabel());
		HitDamagable->OnDamaged(Damage);
	}
}

void ASword::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	IDamagable* HitDamagable = Cast<IDamagable>(OtherActor);
	if (HitDamagable != nullptr && Owner != OtherActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("End, %s"), *OtherActor->GetActorNameOrLabel());
	}
}
