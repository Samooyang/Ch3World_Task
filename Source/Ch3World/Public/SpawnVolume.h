#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemSpawnRow.h"
#include "SpawnVolume.generated.h"

class UBoxComponent;

UCLASS()
class CH3WORLD_API ASpawnVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	ASpawnVolume();
	
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
	USceneComponent* Scene;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
	UBoxComponent* SpawningBox;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	UDataTable* ItemDataTable;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	float SpawnInterval;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	int32 MaxItemCount;
	
	UFUNCTION(BlueprintCallable, Category = "Spawning")
	void SpawnRandomItem();
	
	
	FVector GetRandomPointInVolume() const;
	FItemSpawnRow* GetRandomItem() const;
	void SpawnItem(TSubclassOf<AActor> ItemClass);
	
	
protected:
	virtual void BeginPlay();
	
	
private:
	FTimerHandle SpawnTimerHandle;
	
	int32 SpawnedItemCount;
};
