#include "OBPrimaryLayout.h"
#include "NativeGameplayTags.h"
#include "CommonUIExtensions.h"
#include "Input/CommonUIInputTypes.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OBPrimaryLayout)

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_UI_LAYER_MENU, "UI.Layer.Menu");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_UI_ACTION_ESCAPE, "UI.Action.Escape");

//
// PrimaryLayout
//
UOBPrimaryLayout::UOBPrimaryLayout(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

//
// MenuLayout
//
UOBMenuLayout::UOBMenuLayout(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

//
// GameLayout
//
UOBGameLayout::UOBGameLayout(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UOBGameLayout::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	RegisterUIActionBinding(FBindUIActionArgs(FUIActionTag::ConvertChecked(TAG_UI_ACTION_ESCAPE), false, FSimpleDelegate::CreateUObject(this, &UOBGameLayout::HandleKeyAction)));
}

void UOBGameLayout::HandleKeyAction()
{
	if (ensure(!KeyActionClass.IsNull()))
	{
		UCommonUIExtensions::PushStreamedContentToLayer_ForPlayer(GetOwningLocalPlayer(), TAG_UI_LAYER_MENU, KeyActionClass);
	}
}
