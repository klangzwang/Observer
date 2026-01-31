#include "OBActivatable.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OBActivatable)

UOBActivatable::UOBActivatable(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UOBActivatable::NativeOnActivated()
{
    Super::NativeOnActivated();

    if (IntroAnimation)
    {
        PlayAnimation(IntroAnimation);
    }
}

void UOBActivatable::NativeOnDeactivated()
{
	StartExitTransition();
}

void UOBActivatable::StartExitTransition()
{
    if (OutroAnimation)
    {
        FWidgetAnimationDynamicEvent EndDelegate;
        EndDelegate.BindDynamic(this, &UOBActivatable::OnOutroFinished);
        BindToAnimationFinished(OutroAnimation, EndDelegate);

        PlayAnimation(OutroAnimation);
    }
    else
    {
        DeactivateWidget();

        Super::NativeOnDeactivated();
    }
}

void UOBActivatable::OnOutroFinished()
{
    DeactivateWidget();

    Super::NativeOnDeactivated();
}

TOptional<FUIInputConfig> UOBActivatable::GetDesiredInputConfig() const
{
	switch (InputConfig)
	{
		case EOBInputMode::GameAndMenu:
			return FUIInputConfig(ECommonInputMode::All, GameMouseCaptureMode);
		case EOBInputMode::Game:
			return FUIInputConfig(ECommonInputMode::Game, GameMouseCaptureMode);
		case EOBInputMode::Menu:
			return FUIInputConfig(ECommonInputMode::Menu, EMouseCaptureMode::NoCapture);
		case EOBInputMode::Default:
		default:
			return TOptional<FUIInputConfig>();
	}
}
