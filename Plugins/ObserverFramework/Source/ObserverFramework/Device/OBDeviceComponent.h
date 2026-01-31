#pragma once
#include "Components/GameStateComponent.h"
#include "OBDeviceComponent.generated.h"

class AActor;

UCLASS(Abstract)
class UOBDeviceComponent : public UGameStateComponent
{
	GENERATED_BODY()

public:

	UOBDeviceComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintPure, Category = "Observer|Device")
	static UOBDeviceComponent* FindDeviceComponent(const AGameStateBase* GameState) { return (GameState ? GameState->FindComponentByClass<UOBDeviceComponent>() : nullptr); }

public:

	UPROPERTY(BlueprintReadOnly, Transient, Category = "Observer|Device")
	TArray<AActor*> Devices;
	
public:

	UPROPERTY(BlueprintReadOnly, Transient, Category = "Observer|Device")
	TArray<AActor*> ActiveCapturePoints;
};
