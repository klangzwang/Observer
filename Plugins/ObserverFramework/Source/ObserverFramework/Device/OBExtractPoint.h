#pragma once
#include "OBDeviceBase.h"
#include "OBExtractPoint.generated.h"

UCLASS()
class AOBExtractPoint : public AOBDeviceBase
{
	GENERATED_BODY()
	
public:	

	AOBExtractPoint();

protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
