#pragma once
#include "CommonActivatableWidget.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "GameplayTagContainer.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "OBPrimaryGameLayout.generated.h"

UCLASS(MinimalAPI, Abstract, meta = (DisableNativeTick))
class UOBPrimaryGameLayout : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = "Observer|GameLayout")
	static UOBPrimaryGameLayout* GetPrimaryGameLayoutForPrimaryPlayer(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintCallable, Category = "Observer|GameLayout")
	static UOBPrimaryGameLayout* GetPrimaryGameLayout(APlayerController* PlayerController);
	static UOBPrimaryGameLayout* GetPrimaryGameLayout(ULocalPlayer* LocalPlayer);
	
public:

	UOBPrimaryGameLayout(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;

protected:

	UPROPERTY(meta = (BindWidget))
	UCommonActivatableWidgetStack* GameLayer;

	UPROPERTY(meta = (BindWidget))
	UCommonActivatableWidgetStack* GameMenuLayer;

	UPROPERTY(meta = (BindWidget))
	UCommonActivatableWidgetStack* MenuLayer;

	UPROPERTY(meta = (BindWidget))
	UCommonActivatableWidgetStack* ModalLayer;

public:

	template <typename ActivatableWidgetT = UCommonActivatableWidget>
	ActivatableWidgetT* PushWidgetToLayerStack(FGameplayTag LayerName, TSoftClassPtr<UCommonActivatableWidget> ActivatableWidgetClass, bool PushASync)
	{
		return PushWidgetToLayerStack<ActivatableWidgetT>(LayerName, ActivatableWidgetClass, PushASync, [](ActivatableWidgetT&) {});
	}

	template <typename ActivatableWidgetT = UCommonActivatableWidget>
	ActivatableWidgetT* PushWidgetToLayerStack(FGameplayTag LayerName, TSoftClassPtr<UCommonActivatableWidget> ActivatableWidgetClass, bool PushASync, TFunctionRef<void(ActivatableWidgetT&)> InitInstanceFunc)
	{
		static_assert(TIsDerivedFrom<ActivatableWidgetT, UCommonActivatableWidget>::IsDerived, "Only CommonActivatableWidgets can be used here");

		if (PushASync)
		{
			FStreamableManager& StreamableManager = UAssetManager::Get().GetStreamableManager();
			StreamableManager.RequestAsyncLoad(ActivatableWidgetClass.ToSoftObjectPath(),
				FStreamableDelegate::CreateWeakLambda(this, [this, LayerName, ActivatableWidgetClass, InitInstanceFunc]()
					{
						if (UCommonActivatableWidgetContainerBase* AsyncLayer = GetLayerWidget(LayerName))
						{
							AsyncLayer->AddWidget<ActivatableWidgetT>(ActivatableWidgetClass.Get(), InitInstanceFunc);
						}
					})
			);

			return nullptr;
		}
		else
		{
			if (UCommonActivatableWidgetContainerBase* Layer = GetLayerWidget(LayerName))
			{
				return Layer->AddWidget<ActivatableWidgetT>(ActivatableWidgetClass.Get(), InitInstanceFunc);
			}

			return nullptr;
		}
	}

	void FindAndRemoveWidgetFromLayer(UCommonActivatableWidget* ActivatableWidget);
	
	UFUNCTION()
	UCommonActivatableWidgetContainerBase* GetLayerWidget(FGameplayTag LayerName);

private:

	UPROPERTY(Transient, meta = (Categories = "UI.Layer"))
	TMap<FGameplayTag, TObjectPtr<UCommonActivatableWidgetContainerBase>> Layers;
};
