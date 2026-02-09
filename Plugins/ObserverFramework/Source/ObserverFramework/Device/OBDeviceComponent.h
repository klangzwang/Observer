#pragma once
#include "Components/GameStateComponent.h"
#include "OBDeviceComponent.generated.h"

class AOBDeviceBase;

UCLASS(MinimalAPI, Blueprintable, Meta = (BlueprintSpawnableComponent))
class UOBDeviceComponent : public UGameStateComponent
{
	GENERATED_BODY()

public:

	UOBDeviceComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintPure, Category = "Observer|Device")
	static UOBDeviceComponent* FindDeviceComponent(const AGameStateBase* GameState) { return (GameState ? GameState->FindComponentByClass<UOBDeviceComponent>() : nullptr); }

public:

	UPROPERTY(BlueprintReadOnly, Transient, Category = "Observer|Device")
	TArray<AOBDeviceBase*> Devices;

public:

	UFUNCTION(BlueprintCallable, Category = "Observer|Device")
	AOBDeviceBase* GetDeviceByNumber(int32 DeviceIndex) const { return Devices.IsValidIndex(DeviceIndex) ? Devices[DeviceIndex] : nullptr; }

	UFUNCTION(BlueprintCallable, Category = "Observer|Device")
	AOBDeviceBase* GetDeviceByName(FName InDeviceName) const;

	UFUNCTION(BlueprintCallable, Category = "Observer|Device")
	void OnUpdateProgress(int32 InCurrentProgress);

	UFUNCTION(BlueprintCallable, Category = "Observer|Device")
	void OnUpdateSegment(int32 InCurrentSegment);

	UFUNCTION(BlueprintCallable, Category = "Observer|Device")
	void OnUpdateStatus(bool InNewStatus);
};
