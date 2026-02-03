#pragma once
#include "CoreMinimal.h"
#include "OBActivatable.h"
#include "OBPrimaryLayout.generated.h"

//
// PrimaryLayout
//
UCLASS(Abstract, BlueprintType, Blueprintable)
class UOBPrimaryLayout : public UOBActivatable
{
	GENERATED_BODY()

public:

	UOBPrimaryLayout(const FObjectInitializer& ObjectInitializer);
};

//
// MenuLayout
//
UCLASS(Abstract, BlueprintType, Blueprintable)
class UOBMenuLayout : public UOBPrimaryLayout
{
	GENERATED_BODY()

public:

	UOBMenuLayout(const FObjectInitializer& ObjectInitializer);

	virtual void NativeOnInitialized() override;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Observer|Widget")
	TSoftClassPtr<UCommonActivatableWidget> KeyActionClass;
};

//
// GameLayout
//
UCLASS(Abstract, BlueprintType, Blueprintable)
class UOBGameLayout : public UOBPrimaryLayout
{
	GENERATED_BODY()

public:

	UOBGameLayout(const FObjectInitializer& ObjectInitializer);

	virtual void NativeOnInitialized() override;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Observer|Widget")
	TSoftClassPtr<UCommonActivatableWidget> GameHUDClass;
};
