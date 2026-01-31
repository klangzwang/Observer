#pragma once
#include "CoreMinimal.h"
#include "OBActivatable.h"
#include "OBPressStart.generated.h"

UCLASS(Abstract, BlueprintType, Blueprintable)
class UOBPressStart : public UOBActivatable
{
	GENERATED_BODY()

public:

	UOBPressStart(const FObjectInitializer& ObjectInitializer);

	virtual void NativeOnInitialized() override;
	virtual void NativeOnActivated() override;
	virtual void NativeDestruct() override;

protected:

	FUIActionBindingHandle Handle;

	UFUNCTION(BlueprintImplementableEvent)
	void OnKeyPressed();

	UFUNCTION(BlueprintImplementableEvent)
	void OnKeyProgressed(float Seconds);

	UFUNCTION(BlueprintImplementableEvent)
	void OnKeyReleased();

	UFUNCTION(BlueprintImplementableEvent)
	void OnKeyExecuted();
};
