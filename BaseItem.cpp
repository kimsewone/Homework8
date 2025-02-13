
#include "BaseItem.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
ABaseItem::ABaseItem()
{
	PrimaryActorTick.bCanEverTick = false;
	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Collision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	Collision->SetupAttachment(Scene);

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(Collision);

	Collision->OnComponentBeginOverlap.AddDynamic(this, &ABaseItem::OnItemOverlap);
	Collision->OnComponentEndOverlap.AddDynamic(this, &ABaseItem::OnItemEndOverlap);
}

void ABaseItem::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseItem::BeginDestroy()
{
	//GetWorldTimerManager().ClearAllTimersForObject(this);
	Super::BeginDestroy();
}

void ABaseItem::OnItemOverlap(UPrimitiveComponent* OverlappedComp,/*자신*/
	AActor* OtherActor,/*충돌한 액터*/
	UPrimitiveComponent* OtherComp,/*충돌한 액터에 달린 충돌 컴포넌트(Collision)*/
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->ActorHasTag("Player"))
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("OVerlap!!!")));
		ActivateItem(OtherActor);
	}
}

void ABaseItem::OnItemEndOverlap(UPrimitiveComponent* OverlappedComp,/*자신*/
	AActor* OtherActor,/*충돌한 액터*/
	UPrimitiveComponent* OtherComp,/*충돌한 액터에 달린 충돌 컴포넌트(Collision)*/
	int32 OtherBodyIndex)
{

}

void ABaseItem::ActivateItem(AActor* Acitivator)
{
	UParticleSystemComponent* Particle = nullptr;

	if (PickupParticle)
	{
		Particle = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			PickupParticle,
			GetActorLocation(),
			GetActorRotation(),
			true
		);
	}
	if (PickupSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			PickupSound,
			GetActorLocation()
			);
	}
	if (Particle)
	{
		float ParticleClearTime = 2.f;
		if (ItemType == "Mine")
			ParticleClearTime = 2.5f;
		
		TWeakObjectPtr<ABaseItem> WeakThis(this);
		FTimerHandle DestroyParticleTimerHandle;

		GetWorld()->GetTimerManager().SetTimer(
			DestroyParticleTimerHandle,
			[Particle, WeakThis]()
			{
				if (IsValid(Particle) && WeakThis.IsValid())
				{
					Particle->DestroyComponent();//Deactivate();
					UE_LOG(LogTemp, Warning, TEXT("Particle!!"));
				}
			},
			ParticleClearTime,
			false
		);
	}
}

FName ABaseItem::GetItemType() const
{
	return ItemType;
}

void ABaseItem::DestroyItem()
{
	Destroy();
	UE_LOG(LogTemp, Warning, TEXT("Destroy!!"));
}

