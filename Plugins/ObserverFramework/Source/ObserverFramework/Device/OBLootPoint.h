#pragma once
#include "OBDeviceBase.h"
#include "OBLootPoint.generated.h"

UCLASS()
class AOBLootPoint : public AOBDeviceBase
{
	GENERATED_BODY()
	
public:	

	AOBLootPoint(const FObjectInitializer& ObjectInitializer);

protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
