#include "MineItem.h"
#include "Components/SphereComponent.h"

AMineItem::AMineItem()
{
	ExplosionCollision = CreateDefaultSubobject<USphereComponent>(TEXT("ExplosionCollision"));
	ExplosionCollision->InitSphereRadius(ExplosionRadius);
	ExplosionCollision->SetCollisionProfileName(TEXT("OverlapALlDynamic"));
	ExplosionCollision->SetupAttachment(Scene);
	
	ExplosionDelay = 3.0f;
	ExplosionRadius = 300.0f;
	ExplosionDamage = 30.0f;
	ItemType = "Mine";
}

void AMineItem::ActivateItem(AActor* Activator)
{
	TArray<AActor*> OverlappingActors;
	ExplosionCollision->GetOverlappingActors(OverlappingActors);
	
	for (AActor* Actor : OverlappingActors)
	{
		if (Actor && Actor->ActorHasTag("Player"))
		{
			//test용 텍스트
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green,
				FString::Printf(TEXT("Player Damaged %f by MineItem"), ExplosionDamage));
		}
	}
	DestroyItem();
}