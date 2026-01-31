#pragma once
#include "OBDeviceBase.h"
#include "OBLootPoint.generated.h"

UCLASS()
class AOBLootPoint : public AOBDeviceBase
{
	GENERATED_BODY()
	
public:	

	AOBLootPoint();

protected:

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};