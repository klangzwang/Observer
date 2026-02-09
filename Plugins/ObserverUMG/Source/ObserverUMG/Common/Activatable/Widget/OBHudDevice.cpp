#include "OBHudDevice.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OBHudDevice)

//
// HudDevice
//
UOBHudDevice::UOBHudDevice(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

//
// ExtractPoint
//
UOBHudExtractDevice::UOBHudExtractDevice(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UOBHudExtractDevice::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

//
// CapturePoint
//
UOBHudCaptureDevice::UOBHudCaptureDevice(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UOBHudCaptureDevice::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

//
// LootPoint
//
UOBHudLootDevice::UOBHudLootDevice(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UOBHudLootDevice::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}
