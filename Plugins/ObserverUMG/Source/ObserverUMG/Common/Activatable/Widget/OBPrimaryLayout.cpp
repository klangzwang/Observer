#include "OBPrimaryLayout.h"
#include "CommonUIExtensions.h"
#include "NativeGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OBPrimaryLayout)

UE_DEFINE_GAMEPLAY_TAG_STATIC(LayerTag_Game, "UI.Layer.Game");
UE_DEFINE_GAMEPLAY_TAG_STATIC(LayerTag_GameMenu, "UI.Layer.GameMenu");
UE_DEFINE_GAMEPLAY_TAG_STATIC(LayerTag_Menu, "UI.Layer.Menu");
UE_DEFINE_GAMEPLAY_TAG_STATIC(LayerTag_Modal, "UI.Layer.Modal");

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

void UOBMenuLayout::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (KeyActionClass != nullptr)
		UCommonUIExtensions::PushContentToLayer_ForPlayer(GetOwningLocalPlayer(), LayerTag_Menu, KeyActionClass, true);
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

	if (GameHUDClass != nullptr)
		UCommonUIExtensions::PushContentToLayer_ForPlayer(GetOwningLocalPlayer(), LayerTag_Game, GameHUDClass, true);
}
