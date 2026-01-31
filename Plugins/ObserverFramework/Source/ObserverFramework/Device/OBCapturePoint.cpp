#include "OBCapturePoint.h"
#include "OBDeviceComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OBCapturePoint)

AOBCapturePoint::AOBCapturePoint()
{
}

void AOBCapturePoint::BeginPlay()
{
	Super::BeginPlay();
}

void AOBCapturePoint::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AOBCapturePoint::SetActiveAndRegister()
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
				DeviceManager->ActiveCapturePoints.Add(this);
				IsActivated = true;
			}
		}
	}
}
