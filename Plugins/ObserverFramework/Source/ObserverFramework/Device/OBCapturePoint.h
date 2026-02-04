#pragma once
#include "OBDeviceBase.h"
#include "OBCapturePoint.generated.h"

UENUM(BlueprintType)
enum class ECapturePointState : uint8
{
	Neutral,
	Capturing,
	Contested,
	Captured
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCaptureStateChanged, ECapturePointState, NewState, AController*, Controller);

UCLASS()
class OBSERVERFRAMEWORK_API AOBCapturePoint : public AOBDeviceBase
{
	GENERATED_BODY()

public:
	
	AOBCapturePoint();

protected:
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;

public:
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Observer|Device")
	bool IsActivated = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Observer|Capture")
	float CaptureRadius = 500.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Observer|Capture")
	float CaptureTime = 12.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Observer|Capture")
	ECapturePointState CurrentState = ECapturePointState::Neutral;

	UPROPERTY(BlueprintAssignable, Category = "Observer|Capture")
	FOnCaptureStateChanged OnCaptureStateChanged;

	UFUNCTION(BlueprintCallable, Category = "Observer|Device")
	void SetActiveAndRegister();

	UFUNCTION(BlueprintCallable, Category = "Observer|Capture")
	float GetCaptureProgress() const { return CurrentCaptureProgress; }

protected:
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetCaptureState(ECapturePointState NewState, AController* CapturingController);

private:
	
	float CurrentCaptureProgress = 0.0f;
	int32 TeamAllyCount = 0;
	int32 TeamEnemyCount = 0;

	void UpdateNearbyPlayers();
	void UpdateCaptureProgress();
	void ResetCaptureProgress();
};
