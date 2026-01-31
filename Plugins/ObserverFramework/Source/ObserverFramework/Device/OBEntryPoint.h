#pragma once
#include "OBDeviceBase.h"
#include "OBEntryPoint.generated.h"

UCLASS()
class AOBEntryPoint : public AOBDeviceBase
{
	GENERATED_BODY()

public:

	AOBEntryPoint();

protected:

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};