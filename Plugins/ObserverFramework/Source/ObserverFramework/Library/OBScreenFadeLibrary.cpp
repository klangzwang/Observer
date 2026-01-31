#include "OBScreenFadeLibrary.h"
#include "OBPlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"

void UOBScreenFadeLibrary::FadeDelegate(UObject* WorldContextObject, bool bFadeToBlack)
{
	if (UWorld* World = WorldContextObject->GetWorld())
	{
		if (APlayerCameraManager* Manager = UGameplayStatics::GetPlayerCameraManager(World, 0))
			Cast<AOBPlayerCameraManager>(Manager)->OnBlackFadeDelegate.Broadcast(bFadeToBlack);
	}
}
