#pragma once
#include "UserSettings/EnhancedInputUserSettings.h"
#include "PlayerMappableKeySettings.h"
#include "OBInputUserSettings.generated.h"

/** 
 * Custom settings class for any input related settings for the Lyra game.
 * This will be serialized out at the same time as the Lyra Shared Settings and is
 * compatible with cloud saves through by calling the "Serialize" function.
 */
UCLASS()
class OBSERVERINPUT_API UOBInputUserSettings : public UEnhancedInputUserSettings
{
	GENERATED_BODY()
	
public:

	virtual void ApplySettings() override;
};

/**
 * Player Mappable Key settings are settings that are accessible per-action key mapping.
 * This is where you could place additional metadata that may be used by your settings UI,
 * input triggers, or other places where you want to know about a key setting.
 */
UCLASS()
class OBSERVERINPUT_API ULOBPlayerMappableKeySettings : public UPlayerMappableKeySettings
{
	GENERATED_BODY()
	
public:

	/** Returns the tooltip that should be displayed on the settings screen for this key */
	const FText& GetTooltipText() const;

protected:
	/** The tooltip that should be associated with this action when displayed on the settings screen */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta=(AllowPrivateAccess=true))
	FText Tooltip = FText::GetEmpty();
};