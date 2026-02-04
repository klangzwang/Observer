#pragma once
#include "OBDeviceBase.h"
#include "OBExtractPoint.generated.h"

UENUM(BlueprintType)
enum class EExtractPointState : uint8
{
	Idle,
	Extracting,
	Interrupted,
	Completed,
	Failed
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnExtractStateChanged, EExtractPointState, NewState, float, Progress);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnExtractSegmentComplete, int32, CurrentSegment);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBossAlerted);

UCLASS()
class OBSERVERFRAMEWORK_API AOBExtractPoint : public AOBDeviceBase
{
	GENERATED_BODY()
	
public:	
	
	AOBExtractPoint();

protected:
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Observer|Extract")
	float SegmentDuration = 25.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Observer|Extract")
	int32 TotalSegments = 7;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Observer|Extract")
	float DetectionRadius = 3000.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Observer|Extract")
	EExtractPointState CurrentState = EExtractPointState::Idle;

	UPROPERTY(BlueprintAssignable, Category = "Observer|Extract")
	FOnExtractStateChanged OnExtractStateChanged;

	UPROPERTY(BlueprintAssignable, Category = "Observer|Extract")
	FOnExtractSegmentComplete OnExtractSegmentComplete;

	UPROPERTY(BlueprintAssignable, Category = "Observer|Extract")
	FOnBossAlerted OnBossAlerted;

	UFUNCTION(BlueprintCallable, Category = "Observer|Extract")
	void StartExtraction();

	UFUNCTION(BlueprintCallable, Category = "Observer|Extract")
	void InterruptExtraction();

	UFUNCTION(BlueprintCallable, Category = "Observer|Extract")
	float GetExtractionProgress() const { return CurrentExtractionProgress; }

	UFUNCTION(BlueprintCallable, Category = "Observer|Extract")
	int32 GetCurrentSegment() const { return CurrentSegment; }

	UFUNCTION(BlueprintCallable, Category = "Observer|Extract")
	float GetKIAggression() const { return KIAggressionMultiplier; }

protected:
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_StartExtraction();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_InterruptExtraction();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayExtractionSound();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_AlertBoss();

private:
	
	float CurrentExtractionProgress = 0.0f;
	int32 CurrentSegment = 0;
	float KIAggressionMultiplier = 1.0f;
	bool bLootPlaced = false;

	void UpdateExtractionProgress(float DeltaTime);
	void UpdateKIAggression();
	void CompleteExtraction();
	void ResetExtraction();
};
