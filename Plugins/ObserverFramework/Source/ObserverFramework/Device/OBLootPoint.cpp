#include "OBLootPoint.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OBLootPoint)

AOBLootPoint::AOBLootPoint()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	Tags.Add(FName("Loot"));
}

void AOBLootPoint::BeginPlay()
{
	Super::BeginPlay();

	RegisterDevice(this);
}

void AOBLootPoint::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}