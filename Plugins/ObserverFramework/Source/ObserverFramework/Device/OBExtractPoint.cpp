#include "OBExtractPoint.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OBExtractPoint)

AOBExtractPoint::AOBExtractPoint()
{
}

void AOBExtractPoint::BeginPlay()
{
	Super::BeginPlay();

	RegisterDevice(this);
}

void AOBExtractPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AOBExtractPoint::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}
