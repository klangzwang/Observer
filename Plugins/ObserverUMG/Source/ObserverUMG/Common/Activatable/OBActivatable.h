#pragma once
#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "OBActivatable.generated.h"

UENUM(BlueprintType)
enum class EOBInputMode : uint8
{
	Default,
	GameAndMenu,
	Game,
	Menu
};

UCLASS(Abstract, Blueprintable, meta = (Category = Observer))
class UOBActivatable : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:

	UOBActivatable(const FObjectInitializer& ObjectInitializer);

	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;
	
protected:

    virtual void NativeOnActivated() override;
    virtual void NativeOnDeactivated() override;
	
protected:

    UPROPERTY(BlueprintReadWrite, Transient, meta = (BindWidgetAnimation, OptionalWidget = true))
    UWidgetAnimation* IntroAnimation;
	
    UPROPERTY(BlueprintReadWrite, Transient, meta = (BindWidgetAnimation, OptionalWidget = true))
    UWidgetAnimation* OutroAnimation;
	
protected:

    // Diese Funktion rufen wir auf, wenn wir das Widget schließen wollen
    UFUNCTION(BlueprintCallable, Category = "UI")
    void StartExitTransition();

    // Der Callback, der feuert, wenn die Animation zu Ende ist
    UFUNCTION()
    void OnOutroFinished();

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Observer|Input")
	EOBInputMode InputConfig = EOBInputMode::Default;

	UPROPERTY(EditDefaultsOnly, Category = "Observer|Input")
	EMouseCaptureMode GameMouseCaptureMode = EMouseCaptureMode::CapturePermanently;
};
