#pragma once
#include "OBDeviceBase.h"
#include "OBLootPoint.generated.h"

UENUM(BlueprintType)
enum class ELootPointState : uint8
{
	Locked,
	Unlocking,
	Unlocked,
	Looted
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLootStateChanged, ELootPointState, NewState, AController*, UnlockingController);

UCLASS()
class OBSERVERFRAMEWORK_API AOBLootPoint : public AOBDeviceBase
{
	GENERATED_BODY()
	
public:	

	AOBLootPoint();

protected:

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;

public:
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Observer|Loot")
	float UnlockDuration = 20.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Observer|Loot")
	ELootPointState CurrentState = ELootPointState::Locked;

	UPROPERTY(BlueprintAssignable, Category = "Observer|Loot")
	FOnLootStateChanged OnLootStateChanged;

	UFUNCTION(BlueprintCallable, Category = "Observer|Loot")
	void StartUnlock(AController* KeyHolder);

	UFUNCTION(BlueprintCallable, Category = "Observer|Loot")
	void CancelUnlock();

	UFUNCTION(BlueprintCallable, Category = "Observer|Loot")
	float GetUnlockProgress() const { return CurrentUnlockProgress; }

	UFUNCTION(BlueprintCallable, Category = "Observer|Loot")
	bool HasKeyRequired(AController* KeyHolder) const;

protected:
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_StartUnlock(AController* KeyHolder);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_CancelUnlock();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayAlertSound();

private:
	
	float CurrentUnlockProgress = 0.0f;
	TWeakObjectPtr<AController> CurrentUnlocker;

	void UpdateUnlockProgress(float DeltaTime);
	void CompleteUnlock();
	void ResetUnlock();
};
