#include "OBActivatable.h"
#include "Input/CommonUIInputTypes.h"
#include "NativeGameplayTags.h"
#include "Input/CommonUIActionRouterBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OBActivatable)

UE_DEFINE_GAMEPLAY_TAG_STATIC(ActionTag_Back, "UI.Action.Back");
UE_DEFINE_GAMEPLAY_TAG_STATIC(ActionTag_Start, "UI.Action.Start");

UOBActivatable::UOBActivatable(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UOBActivatable::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (UIActionBindings.Num() == 0)
		return;

	for (FGameplayTag ActionBind : UIActionBindings)
	{
		FBindUIActionArgs BindArgs(FBindUIActionArgs(FUIActionTag::ConvertChecked(ActionBind), false, FSimpleDelegate::CreateUObject(this, &ThisClass::OnKeyExecuted)));

		BindArgs.OnHoldActionPressed.BindUObject(this, &ThisClass::OnKeyPressed);
		BindArgs.OnHoldActionProgressed.BindUObject(this, &ThisClass::OnKeyProgressed);
		BindArgs.OnHoldActionReleased.BindUObject(this, &ThisClass::OnKeyReleased);
		BindArgs.OnExecuteAction.BindUObject(this, &ThisClass::OnKeyExecuted);

		if (UCommonUIActionRouterBase* ActionRouter = UCommonUIActionRouterBase::Get(*this))
		{
			FUIActionBindingHandle BindingHandle = ActionRouter->RegisterUIActionBinding(*this, BindArgs);
			AddActionBinding(BindingHandle);
		}
	}
}

void UOBActivatable::NativeOnActivated()
{
	StartIntroTransition();
}

void UOBActivatable::NativeOnDeactivated()
{
	StartExitTransition();
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

//
// Animation
//
FWidgetAnimationHandle UOBActivatable::PlayAnim(UWidgetAnimation* InAnimation, float StartAtTime, int32 NumLoopsToPlay, EUMGSequencePlayMode::Type PlayMode, float PlaybackSpeed)
{
	return PlayAnimation(InAnimation, StartAtTime, NumLoopsToPlay, PlayMode, PlaybackSpeed);
}

void UOBActivatable::SetAnimation(UWidgetAnimation* InIntroAnim, UWidgetAnimation* InOutroAnim)
{
	if (InIntroAnim != nullptr)
	{
		if (IntroAnimation != InIntroAnim)
			IntroAnimation = InIntroAnim;
	}
	if (InOutroAnim != nullptr)
	{
		if (OutroAnimation != InOutroAnim)
			OutroAnimation = InOutroAnim;
	}
}

void UOBActivatable::StartIntroTransition()
{
	if (IntroAnimation)
	{
		FWidgetAnimationDynamicEvent StartDelegate;
		StartDelegate.BindDynamic(this, &UOBActivatable::OnIntroStarted);
		BindToAnimationStarted(IntroAnimation, StartDelegate);

		PlayAnim(IntroAnimation);
	}
	else
	{
		ActivateWidget();

		Super::NativeOnActivated();
	}
}

void UOBActivatable::OnIntroStarted()
{
	ActivateWidget();

	Super::NativeOnActivated();
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
