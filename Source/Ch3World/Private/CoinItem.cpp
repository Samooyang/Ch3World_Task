#include "CoinItem.h"

ACoinItem::ACoinItem()
{
	PointValue = 0;
	ItemType = "DefaultCoin";
}

void ACoinItem::ActivateItem(AActor* Activator)
{
	if (Activator && Activator->ActorHasTag("Player"))
	{
		//test용 텍스트
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, 
			FString::Printf(TEXT("Player gained %d points!"), PointValue));
		
		DestroyItem();
	}
}