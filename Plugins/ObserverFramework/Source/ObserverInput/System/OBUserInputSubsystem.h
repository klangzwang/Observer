#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Framework/Application/IInputProcessor.h"
#include "OBUserInputSubsystem.generated.h"

UENUM(BlueprintType)
enum class EInputBlockMode : uint8
{
    None,
    OnlyUI,
    Cinematic,
    TotalBlock
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInputBlockModeChanged, EInputBlockMode, NewMode);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHoldActionComplete);

class OBSERVERINPUT_API FGlobalInputProcessor : public IInputProcessor
{
public:
    virtual bool HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override;
    virtual bool HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override;
    virtual bool HandleAnalogInputEvent(FSlateApplication& SlateApp, const FAnalogInputEvent& InAnalogInputEvent) override;
    virtual bool HandleMouseButtonDownEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override;
    virtual bool HandleMouseMoveEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override;
    virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override;

    EInputBlockMode CurrentMode = EInputBlockMode::None;
    TArray<FKey> Whitelist;

    // Hold-to-Skip Logic
    FKey KeyToTrack = EKeys::Invalid;
    float RequiredHoldTime = 1.5f;
    float CurrentHoldTime = 0.0f;
    bool bIsTrackedKeyDown = false;
    TFunction<void()> OnHoldActionCompleted;
};

UCLASS()
class OBSERVERINPUT_API UOBUserInputSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    UFUNCTION(BlueprintCallable, Category = "Input")
    void SetInputBlockMode(EInputBlockMode NewMode);

    UFUNCTION(BlueprintCallable, Category = "Input")
    void SetupHoldToSkip(FKey Key, float Duration);

    UFUNCTION(BlueprintPure, Category = "Input")
    float GetHoldProgress() const;

    UPROPERTY(BlueprintAssignable, Category = "Input")
    FOnInputBlockModeChanged OnInputBlockModeChanged;

    UPROPERTY(BlueprintAssignable, Category = "Input")
    FOnHoldActionComplete OnHoldActionComplete;

private:
    TSharedPtr<FGlobalInputProcessor> Processor;
};