#pragma once
#include "CoreMinimal.h"
#include "OBActivatable.h"
#include "OBHudDevice.generated.h"

//
// HudDevice
//
UCLASS(Abstract, BlueprintType, NotBlueprintable)
class UOBHudDevice : public UOBActivatable
{
	GENERATED_BODY()

public:

	UOBHudDevice(const FObjectInitializer& ObjectInitializer);
};

//
// ExtractPoint
//
UCLASS(Abstract, BlueprintType, Blueprintable)
class UOBHudExtractDevice : public UOBHudDevice
{
	GENERATED_BODY()

public:

	UOBHudExtractDevice(const FObjectInitializer& ObjectInitializer);

	virtual void NativeOnInitialized() override;

public:

	UFUNCTION(BlueprintImplementableEvent, Category = "Observer|Device", meta = (DisplayName = "OnUpdateProgess"))
	void K2_OnUpdateProgess(int32 CurrentProgress, int32 CurrentSegment);
};

//
// CapturePoint
//
UCLASS(Abstract, BlueprintType, Blueprintable)
class UOBHudCaptureDevice : public UOBHudDevice
{
	GENERATED_BODY()

public:

	UOBHudCaptureDevice(const FObjectInitializer& ObjectInitializer);

	virtual void NativeOnInitialized() override;

public:

	UFUNCTION(BlueprintImplementableEvent, Category = "Observer|Device", meta = (DisplayName = "OnUpdateProgess"))
	void K2_OnUpdateProgess(int32 CurrentProgress);
};

//
// LootPoint
//
UCLASS(Abstract, BlueprintType, Blueprintable)
class UOBHudLootDevice : public UOBHudDevice
{
	GENERATED_BODY()

public:

	UOBHudLootDevice(const FObjectInitializer& ObjectInitializer);

	virtual void NativeOnInitialized() override;

public:

	UFUNCTION(BlueprintImplementableEvent, Category = "Observer|Device", meta = (DisplayName = "OnUpdateStatus"))
	void K2_OnUpdateStatus(bool bIsLooted);
};

