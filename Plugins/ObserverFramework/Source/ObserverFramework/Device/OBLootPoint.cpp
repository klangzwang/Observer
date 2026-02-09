#include "OBLootPoint.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OBLootPoint)

AOBLootPoint::AOBLootPoint(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DeviceName = "Loot";
}

void AOBLootPoint::BeginPlay()
{
	Super::BeginPlay();

	RegisterDevice(this);
}

void AOBLootPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AOBLootPoint::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}
