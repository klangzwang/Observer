#pragma once
#include "OBDeviceBase.h"
#include "OBCapturePoint.generated.h"

UCLASS()
class AOBCapturePoint : public AOBDeviceBase
{
	GENERATED_BODY()

public:

	AOBCapturePoint();

protected:

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Observer|Device")
	bool IsActivated = false;

	UFUNCTION(BlueprintCallable, Category = "Observer|Device")
	void SetActiveAndRegister();
};