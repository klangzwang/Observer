#pragma once
#include "Engine/World.h"
#include "OBUIGamePolicy.generated.h"

class UOBUIManagerSubsystem;
class UOBPrimaryGameLayout;
class UOBLocalPlayer;
class ULocalPlayer;

UENUM()
enum class ELocalMultiplayerInteractionMode : uint8
{
	// Fullscreen viewport for the primary player only, regardless of the other player's existence
	PrimaryOnly,

	// Fullscreen viewport for one player, but players can swap control over who's is displayed and who's is dormant
	SingleToggle,

	// Viewports displayed simultaneously for both players
	Simultaneous
};

USTRUCT()
struct FRootViewportLayoutInfo
{
	GENERATED_BODY()

public:

	UPROPERTY(Transient)
	TObjectPtr<ULocalPlayer> LocalPlayer = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UOBPrimaryGameLayout> RootLayout = nullptr;

	UPROPERTY(Transient)
	bool bAddedToViewport = false;

	FRootViewportLayoutInfo() {}
	FRootViewportLayoutInfo(ULocalPlayer* InLocalPlayer, UOBPrimaryGameLayout* InRootLayout, bool bIsInViewport)
		: LocalPlayer(InLocalPlayer)
		, RootLayout(InRootLayout)
		, bAddedToViewport(bIsInViewport)
	{}

	bool operator==(const ULocalPlayer* OtherLocalPlayer) const { return LocalPlayer == OtherLocalPlayer; }
};

UCLASS(MinimalAPI, Abstract, Blueprintable, Within = OBUIManagerSubsystem)
class UOBUIGamePolicy : public UObject
{
	GENERATED_BODY()

public:

	template <typename OBUIGamePolicyClass = UOBUIGamePolicy>
	static OBUIGamePolicyClass* GetOBUIGamePolicyAs(const UObject* WorldContextObject)
	{
		return Cast<OBUIGamePolicyClass>(GetOBUIGamePolicy(WorldContextObject));
	}

	static UOBUIGamePolicy* GetOBUIGamePolicy(const UObject* WorldContextObject);

public:

	virtual UWorld* GetWorld() const override;
	UOBUIManagerSubsystem* GetOwningUIManager() const;
	UOBPrimaryGameLayout* GetRootLayout(const UOBLocalPlayer* LocalPlayer) const;

	ELocalMultiplayerInteractionMode GetLocalMultiplayerInteractionMode() const { return LocalMultiplayerInteractionMode; }

	void RequestPrimaryControl(UOBPrimaryGameLayout* Layout);

protected:

	void AddLayoutToViewport(UOBLocalPlayer* LocalPlayer, UOBPrimaryGameLayout* Layout);
	void RemoveLayoutFromViewport(UOBLocalPlayer* LocalPlayer, UOBPrimaryGameLayout* Layout);

	virtual void OnRootLayoutAddedToViewport(UOBLocalPlayer* LocalPlayer, UOBPrimaryGameLayout* Layout);
	virtual void OnRootLayoutRemovedFromViewport(UOBLocalPlayer* LocalPlayer, UOBPrimaryGameLayout* Layout);
	virtual void OnRootLayoutReleased(UOBLocalPlayer* LocalPlayer, UOBPrimaryGameLayout* Layout);

	void CreateLayoutWidget(UOBLocalPlayer* LocalPlayer);
	TSubclassOf<UOBPrimaryGameLayout> GetLayoutWidgetClass(UOBLocalPlayer* LocalPlayer);

private:

	ELocalMultiplayerInteractionMode LocalMultiplayerInteractionMode = ELocalMultiplayerInteractionMode::PrimaryOnly;

	UPROPERTY(EditAnywhere)
	TSoftClassPtr<UOBPrimaryGameLayout> LayoutClass;

	UPROPERTY(Transient)
	TArray<FRootViewportLayoutInfo> RootViewportLayouts;

public:

	void NotifyPlayerAdded(UOBLocalPlayer* LocalPlayer);
	void NotifyPlayerRemoved(UOBLocalPlayer* LocalPlayer);
	void NotifyPlayerDestroyed(UOBLocalPlayer* LocalPlayer);

private:

	friend class UGameUIManagerSubsystem;
};
