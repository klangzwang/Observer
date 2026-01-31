#pragma once
#include "CoreMinimal.h"
#include "OBActivatable.h"
#include "OBGameMenu.generated.h"

UCLASS(Abstract, BlueprintType, Blueprintable, meta = (Category = Observer))
class UOBGameMenu : public UOBActivatable
{
	GENERATED_BODY()

public:

	UOBGameMenu(const FObjectInitializer& ObjectInitializer);

	virtual void NativeOnInitialized() override;
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;

protected:

	void HandleGameMenuAction();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UOBActivatable> BlankWidgetClass;

private:

	UPROPERTY(Transient)
	TWeakObjectPtr<UCommonActivatableWidget> BlankGameWidget;

	UPROPERTY(Transient)
	TWeakObjectPtr<UCommonActivatableWidget> BlankGameMenuWidget;
};
