#include "OBUIManagerSubsystem.h"
#include "OBUIGamePolicy.h"
#include "OBLocalPlayer.h"
#include "OBPrimaryGameLayout.h"
#include "OBHUD.h"
#include "CommonActivatableWidget.h"
#include "OBGameMode.h"
#include "Engine/GameInstance.h"
#include "GameFramework/HUD.h"
#include "Kismet/GameplayStatics.h"

class FSubsystemCollectionBase;
class UClass;

UOBUIManagerSubsystem::UOBUIManagerSubsystem()
	: Super()
{
}

void UOBUIManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (!CurrentPolicy && !DefaultUIPolicyClass.IsNull())
	{
		TSubclassOf<UOBUIGamePolicy> PolicyClass = DefaultUIPolicyClass.LoadSynchronous();
		SwitchToPolicy(NewObject<UOBUIGamePolicy>(this, PolicyClass));
	}

	TickHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &UOBUIManagerSubsystem::Tick), 0.0f);
}

void UOBUIManagerSubsystem::Deinitialize()
{
	Super::Deinitialize();

	SwitchToPolicy(nullptr);

	FTSTicker::GetCoreTicker().RemoveTicker(TickHandle);
}

bool UOBUIManagerSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!CastChecked<UGameInstance>(Outer)->IsDedicatedServerInstance())
	{
		TArray<UClass*> ChildClasses;
		GetDerivedClasses(GetClass(), ChildClasses, false);
		return ChildClasses.Num() == 0;
	}
	return false;
}

void UOBUIManagerSubsystem::NotifyPlayerAdded(UOBLocalPlayer* LocalPlayer)
{
	if (ensure(LocalPlayer) && CurrentPolicy)
	{
		CurrentPolicy->NotifyPlayerAdded(LocalPlayer);
	}
}

void UOBUIManagerSubsystem::NotifyPlayerRemoved(UOBLocalPlayer* LocalPlayer)
{
	if (LocalPlayer && CurrentPolicy)
	{
		CurrentPolicy->NotifyPlayerRemoved(LocalPlayer);
	}
}

void UOBUIManagerSubsystem::NotifyPlayerDestroyed(UOBLocalPlayer* LocalPlayer)
{
	if (LocalPlayer && CurrentPolicy)
	{
		CurrentPolicy->NotifyPlayerDestroyed(LocalPlayer);
	}
}

void UOBUIManagerSubsystem::SwitchToPolicy(UOBUIGamePolicy* InPolicy)
{
	if (CurrentPolicy != InPolicy)
	{
		CurrentPolicy = InPolicy;
	}
}

bool UOBUIManagerSubsystem::Tick(float DeltaTime)
{
	SyncRootLayoutVisibilityToShowHUD();
	
	return true;
}

void UOBUIManagerSubsystem::SyncRootLayoutVisibilityToShowHUD()
{
	if (const UOBUIGamePolicy* Policy = GetCurrentUIPolicy())
	{
		for (const ULocalPlayer* LocalPlayer : GetGameInstance()->GetLocalPlayers())
		{
			bool bShouldShowUI = true;
			
			if (const APlayerController* PC = LocalPlayer->GetPlayerController(GetWorld()))
			{
				const AHUD* HUD = PC->GetHUD();

				if (HUD && !HUD->bShowHUD)
				{
					bShouldShowUI = false;
				}
			}

			if (UOBPrimaryGameLayout* RootLayout = Policy->GetRootLayout(CastChecked<UOBLocalPlayer>(LocalPlayer)))
			{
				const ESlateVisibility DesiredVisibility = bShouldShowUI ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed;
				if (DesiredVisibility != RootLayout->GetVisibility())
				{
					RootLayout->SetVisibility(DesiredVisibility);	
				}
			}
		}
	}
}

/*
void UOBUIManagerSubsystem::PushMenuWidget(TSoftClassPtr<UCommonActivatableWidget> WidgetClass)
{
	if (const UOBUIGamePolicy* Policy = GetCurrentUIPolicy())
	{
		for (const ULocalPlayer* LocalPlayer : GetGameInstance()->GetLocalPlayers())
		{
			FGameplayTag MenuTag = FGameplayTag::RequestGameplayTag(FName("UI.Layer.Menu"));
			if (UOBPrimaryGameLayout* RootLayout = Cast<UOBPrimaryGameLayout>(Policy->GetRootLayout(Cast<UOBLocalPlayer>(LocalPlayer))))
			{
				if (UCommonActivatableWidgetStack* Stack = Cast<UCommonActivatableWidgetStack>(RootLayout->GetLayerWidget(MenuTag)))
				{
					Stack->AddWidget(WidgetClass.LoadSynchronous());
				}
			}
		}
	}
}
*/
