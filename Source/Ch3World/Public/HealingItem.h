#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "HealingItem.generated.h"

UCLASS()
class CH3WORLD_API AHealingItem : public ABaseItem
{
	GENERATED_BODY()
	
public:
	AHealingItem();
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float HealAmount;
	
	virtual void ActivateItem(AActor* Activator) override;
};
