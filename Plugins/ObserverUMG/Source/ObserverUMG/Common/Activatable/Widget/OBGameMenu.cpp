#include "OBGameMenu.h"
#include "CommonUIExtensions.h"
#include "NativeGameplayTags.h"
#include "Input/CommonUIInputTypes.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OBGameMenu)

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_UI_ACTION_ESCAPE, "UI.Action.Escape");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_UI_LAYER_GAME, "UI.Layer.Game");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_UI_LAYER_GAMEMENU, "UI.Layer.GameMenu");

UOBGameMenu::UOBGameMenu(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UOBGameMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	constexpr bool bShouldDisplayInActionBar = false;
	RegisterUIActionBinding(FBindUIActionArgs(FUIActionTag::ConvertChecked(TAG_UI_ACTION_ESCAPE), bShouldDisplayInActionBar, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleGameMenuAction)));
}

void UOBGameMenu::NativeOnActivated()
{
	Super::NativeOnActivated();

	BlankGameWidget = UCommonUIExtensions::PushContentToLayer_ForPlayer(GetOwningLocalPlayer(), TAG_UI_LAYER_GAME, BlankWidgetClass);
	BlankGameMenuWidget = UCommonUIExtensions::PushContentToLayer_ForPlayer(GetOwningLocalPlayer(), TAG_UI_LAYER_GAMEMENU, BlankWidgetClass);
}

void UOBGameMenu::NativeOnDeactivated()
{
	if (BlankGameWidget.IsValid())
	{
		UCommonUIExtensions::PopContentFromLayer(BlankGameWidget.Get());
		BlankGameWidget.Reset();
	}

	if (BlankGameMenuWidget.IsValid())
	{
		UCommonUIExtensions::PopContentFromLayer(BlankGameMenuWidget.Get());
		BlankGameMenuWidget.Reset();
	}

	Super::NativeOnDeactivated();
}

void UOBGameMenu::HandleGameMenuAction()
{
	DeactivateWidget();
}
