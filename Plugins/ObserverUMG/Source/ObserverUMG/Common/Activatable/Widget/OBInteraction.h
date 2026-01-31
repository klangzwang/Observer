#pragma once
#include "CoreMinimal.h"
#include "OBActivatable.h"
#include "OBInteraction.generated.h"

UCLASS(Abstract, BlueprintType, Blueprintable, meta = (Category = Observer))
class UOBInteraction : public UOBActivatable
{
	GENERATED_BODY()

public:

	UOBInteraction(const FObjectInitializer& ObjectInitializer);

	virtual void NativeOnInitialized() override;
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;
};
