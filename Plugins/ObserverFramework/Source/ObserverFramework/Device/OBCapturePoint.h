#pragma once
#include "OBDeviceBase.h"
#include "OBCapturePoint.generated.h"

class APlayerStart;

UCLASS()
class AOBCapturePoint : public AOBDeviceBase
{
	GENERATED_BODY()

public:

	AOBCapturePoint(const FObjectInitializer& ObjectInitializer);

protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Observer|Device")
	int SecondsToCapture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Observer|Device")
	int SecondsCaptureFall;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Observer|Device")
	bool bIsCapturedByPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Observer|Device")
	bool bIsCapturedByEnemy;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Observer|Device")
	APlayerStart* RespawnPlayerStart;
};
