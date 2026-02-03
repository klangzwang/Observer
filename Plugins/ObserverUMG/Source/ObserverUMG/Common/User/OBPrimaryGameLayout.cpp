#include "OBPrimaryGameLayout.h"
#include "OBLocalPlayer.h"
#include "OBUIManagerSubsystem.h"
#include "OBUIGamePolicy.h"
#include "OBGameMode.h"
#include "OBTypes.h"
#include "CommonUIExtensions.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OBPrimaryGameLayout)

class UObject;

/*static*/ UOBPrimaryGameLayout* UOBPrimaryGameLayout::GetPrimaryGameLayoutForPrimaryPlayer(const UObject* WorldContextObject)
{
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject);
	APlayerController* PlayerController = GameInstance->GetPrimaryPlayerController(false);
	return GetPrimaryGameLayout(PlayerController);
}

/*static*/ UOBPrimaryGameLayout* UOBPrimaryGameLayout::GetPrimaryGameLayout(APlayerController* PlayerController)
{
	return PlayerController ? GetPrimaryGameLayout(Cast<UOBLocalPlayer>(PlayerController->Player)) : nullptr;
}

/*static*/ UOBPrimaryGameLayout* UOBPrimaryGameLayout::GetPrimaryGameLayout(ULocalPlayer* LocalPlayer)
{
	if (LocalPlayer)
	{
		const UOBLocalPlayer* CommonLocalPlayer = CastChecked<UOBLocalPlayer>(LocalPlayer);
		if (const UGameInstance* GameInstance = CommonLocalPlayer->GetGameInstance())
		{
			if (UOBUIManagerSubsystem* UIManager = GameInstance->GetSubsystem<UOBUIManagerSubsystem>())
			{
				if (const UOBUIGamePolicy* Policy = UIManager->GetCurrentUIPolicy())
				{
					if (UOBPrimaryGameLayout* RootLayout = Policy->GetRootLayout(CommonLocalPlayer))
					{
						return RootLayout;
					}
				}
			}
		}
	}

	return nullptr;
}

UOBPrimaryGameLayout::UOBPrimaryGameLayout(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UOBPrimaryGameLayout::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Layers.Add(FGameplayTag::RequestGameplayTag(FName("UI.Layer.Game")), GameLayer);
	Layers.Add(FGameplayTag::RequestGameplayTag(FName("UI.Layer.GameMenu")), GameMenuLayer);
	Layers.Add(FGameplayTag::RequestGameplayTag(FName("UI.Layer.Menu")), MenuLayer);
	Layers.Add(FGameplayTag::RequestGameplayTag(FName("UI.Layer.Modal")), ModalLayer);
}

void UOBPrimaryGameLayout::NativeConstruct()
{
	Super::NativeConstruct();

	if (AOBGameMode* OBGameMode = Cast<AOBGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		if (OBGameMode->HUDLayout.LayerID.IsValid() && OBGameMode->HUDLayout.LayoutClass != nullptr)
		{
			UCommonUIExtensions::PushContentToLayer_ForPlayer(
				GetOwningLocalPlayer(),
				OBGameMode->HUDLayout.LayerID,
				OBGameMode->HUDLayout.LayoutClass,
				true
			);
		}
	}
}

void UOBPrimaryGameLayout::FindAndRemoveWidgetFromLayer(UCommonActivatableWidget* ActivatableWidget)
{
	for (const auto& LayerKVP : Layers)
	{
		LayerKVP.Value->RemoveWidget(*ActivatableWidget);
	}
}

UCommonActivatableWidgetContainerBase* UOBPrimaryGameLayout::GetLayerWidget(FGameplayTag LayerName)
{
	return Layers.Find(LayerName)->Get();
}
