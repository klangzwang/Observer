#pragma once
#include "GameFramework/Actor.h"
#include "OBDeviceBase.generated.h"

/**
 * AOBDeviceBase
 *
 *	Base class for all device actors.
 */
UCLASS(MinimalAPI, Blueprintable, BlueprintType)
class AOBDeviceBase : public AActor
{
	GENERATED_BODY()
	
public:	

	AOBDeviceBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	
	virtual void RegisterDevice(AActor* DeviceActor);

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Observer|Device")
	FName DeviceName;
};