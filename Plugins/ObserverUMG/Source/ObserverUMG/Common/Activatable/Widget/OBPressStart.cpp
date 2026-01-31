#include "OBPressStart.h"
#include "NativeGameplayTags.h"
#include "Input/CommonUIInputTypes.h"
#include "Input/CommonUIInputSettings.h"
#include "Input/UIActionBinding.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OBPressStart)

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_UI_ACTION_START, "UI.Action.Start");

UOBPressStart::UOBPressStart(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UOBPressStart::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	FBindUIActionArgs BindArgs(FBindUIActionArgs(FUIActionTag::ConvertChecked(TAG_UI_ACTION_START), false, FSimpleDelegate::CreateUObject(this, &ThisClass::OnKeyExecuted)));

	BindArgs.OnHoldActionPressed.BindUObject(this, &ThisClass::OnKeyPressed);
	BindArgs.OnHoldActionProgressed.BindUObject(this, &ThisClass::OnKeyProgressed);
	BindArgs.OnHoldActionReleased.BindUObject(this, &ThisClass::OnKeyReleased);
	BindArgs.OnExecuteAction.BindUObject(this, &ThisClass::OnKeyExecuted);

	Handle = RegisterUIActionBinding(BindArgs);
}

void UOBPressStart::NativeOnActivated()
{
	Super::NativeOnActivated();
}

void UOBPressStart::NativeDestruct()
{
	Super::NativeDestruct();
}
