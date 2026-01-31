#include "OBEntryPoint.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OBEntryPoint)

AOBEntryPoint::AOBEntryPoint()
{
}

void AOBEntryPoint::BeginPlay()
{
	Super::BeginPlay();

	RegisterDevice(this);
}

void AOBEntryPoint::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}
