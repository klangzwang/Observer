#include "OBDeviceBase.h"
#include "OBDeviceComponent.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OBDeviceBase)

AOBDeviceBase::AOBDeviceBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void AOBDeviceBase::RegisterDevice(AActor* DeviceActor)
{
	if (DeviceName == "")
	{
		UE_LOG(LogTemp, Warning, TEXT("Device [%s] not registered."), *GetNameSafe(DeviceActor));
		return;
	}
	else
	{
		UWorld* World = GetWorld();
		if (World && GetLocalRole() == ROLE_Authority)
		{
			AGameStateBase* GameState = World->GetGameState<AGameStateBase>();
			if (GameState)
			{
				UOBDeviceComponent* DeviceManager = GameState->FindComponentByClass<UOBDeviceComponent>();
				if (DeviceManager)
				{
					DeviceManager->Devices.Add(DeviceActor);
				}
			}
		}
	}
}
