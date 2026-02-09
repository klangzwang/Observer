#pragma once
#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "GameplayTagContainer.h"
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

	UFUNCTION(BlueprintCallable, Category = Hud)
	virtual void SetLifeTime(float Lifespan);

	UFUNCTION(BlueprintCallable, Category = Hud)
	void NotifyHUDWidgetIsDone();

public:

	UOBActivatable(const FObjectInitializer& ObjectInitializer);

	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;


protected:

	FTimerHandle LifeTimer;

protected:

	virtual void NativeOnInitialized() override;
    virtual void NativeOnActivated() override;
    virtual void NativeOnDeactivated() override;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Observer|Input")
	EOBInputMode InputConfig = EOBInputMode::Default;

	UPROPERTY(EditDefaultsOnly, Category = "Observer|Input")
	EMouseCaptureMode GameMouseCaptureMode = EMouseCaptureMode::CapturePermanently;

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Observer|Binding")
	TArray<FGameplayTag> UIActionBindings;

	FUIActionBindingHandle Handle;

	UFUNCTION(BlueprintImplementableEvent)
	void OnKeyPressed();

	UFUNCTION(BlueprintImplementableEvent)
	void OnKeyProgressed(float Seconds);

	UFUNCTION(BlueprintImplementableEvent)
	void OnKeyReleased();

	UFUNCTION(BlueprintImplementableEvent)
	void OnKeyExecuted();

//
// Animation
//
protected:

	UPROPERTY(BlueprintReadWrite, Transient, meta = (BindWidgetAnimation, OptionalWidget = true))
	UWidgetAnimation* IntroAnimation;

	UPROPERTY(BlueprintReadWrite, Transient, meta = (BindWidgetAnimation, OptionalWidget = true))
	UWidgetAnimation* OutroAnimation;

protected:

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Observer|Animation")
	FWidgetAnimationHandle PlayAnim(
		UWidgetAnimation* InAnimation,
		float StartAtTime = 0.0f,
		int32 NumLoopsToPlay = 1,
		EUMGSequencePlayMode::Type PlayMode = EUMGSequencePlayMode::Forward,
		float PlaybackSpeed = 1.0f);

	UFUNCTION(BlueprintCallable, Category = "Observer|Animation")
	void SetAnimation(UWidgetAnimation* InIntroAnim = nullptr, UWidgetAnimation* InOutroAnim = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Observer|Animation")
	void StartIntroTransition();

	UFUNCTION()
	void OnIntroStarted();

	UFUNCTION(BlueprintCallable, Category = "Observer|Animation")
	void StartExitTransition();

	UFUNCTION()
	void OnOutroFinished();
};
