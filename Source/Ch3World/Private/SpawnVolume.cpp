#include "SpawnVolume.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"

ASpawnVolume::ASpawnVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);
	
	SpawningBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawningBox"));
	SpawningBox->SetupAttachment(Scene);
	
	MaxItemCount = 30;
	SpawnedItemCount = 0;
	
	SpawnInterval = 0.5f;
	ItemDataTable = nullptr;
	
}

void ASpawnVolume::BeginPlay()
{
	Super::BeginPlay();
	
	if (SpawnInterval > 0.0f && MaxItemCount > 0)
	{
		GetWorld()->GetTimerManager().SetTimer(
			SpawnTimerHandle,
			this,
			&ASpawnVolume::SpawnRandomItem,
			SpawnInterval,
			true
			);
	}
}


void ASpawnVolume::SpawnRandomItem()
{
	if (SpawnedItemCount >= MaxItemCount)
	{
		GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle);
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Item Count On Map = 30!!!")));
		return;
	}
	
	
	if (FItemSpawnRow* SelectedRow = GetRandomItem())
	{
		if (UClass* ActualClass = SelectedRow->ItemClass.Get())
		{
			SpawnItem(ActualClass);
			SpawnedItemCount++;
		}
	}
}

FVector ASpawnVolume::GetRandomPointInVolume() const
{
	const FVector BoxExtent = SpawningBox->GetScaledBoxExtent();
	const FVector BoxOrigin = SpawningBox->GetComponentLocation();
	
	return BoxOrigin + FVector(
		FMath::FRandRange(-BoxExtent.X, BoxExtent.X),
		FMath::FRandRange(-BoxExtent.Y, BoxExtent.Y),
		FMath::FRandRange(-BoxExtent.Z, BoxExtent.Z)
		);
}

FItemSpawnRow* ASpawnVolume::GetRandomItem() const
{
	if (!ItemDataTable) return nullptr;
	
	TArray<FItemSpawnRow*> AllRows;
	static const FString ContextString(TEXT("ItemSpawnContext"));
	ItemDataTable->GetAllRows(ContextString, AllRows);
	
	if (AllRows.IsEmpty()) return nullptr;
	
	float TotalChance = 0.0f;
	for (const FItemSpawnRow* Row : AllRows)
	{
		if (Row)
		{
			TotalChance += Row->SpawnChance;
		}
	}
	
	const float RandValue = FMath::RandRange(0.0f, TotalChance);
	float AccumulateChance = 0.0f;
	for (FItemSpawnRow* Row : AllRows)
	{
		AccumulateChance += Row->SpawnChance;
		if (RandValue <= AccumulateChance)
		{
			return Row;
		}
	}
	return nullptr;
}

void ASpawnVolume::SpawnItem(TSubclassOf<AActor> ItemClass)
{
	if (!ItemClass) return;
	
	GetWorld()->SpawnActor<AActor>(
		ItemClass,
		GetRandomPointInVolume(),
		FRotator::ZeroRotator
		);
}

//Timer를 사용하여 지속적으로 스폰하게 만들기
