#pragma once
#include "OBDeviceBase.h"
#include "OBExtractPoint.generated.h"

UCLASS()
class AOBExtractPoint : public AOBDeviceBase
{
	GENERATED_BODY()

public:

	AOBExtractPoint(const FObjectInitializer& ObjectInitializer);

protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Observer|Device")
	TMap<uint8, float> SegmentsToProgressMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Observer|Device")
	int CurrentSegment;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Observer|Device")
	int SecondsEachSegment;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Observer|Device")
	int SecondsSegmentFall;
};
