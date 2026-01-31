#include "OBUserInputSubsystem.h"
#include "Framework/Application/SlateApplication.h"
#include "Layout/WidgetPath.h"

// --- PROCESSOR IMPLEMENTATION ---

bool FGlobalInputProcessor::HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent)
{
    if (InKeyEvent.GetKey() == KeyToTrack) bIsTrackedKeyDown = true;
    if (CurrentMode == EInputBlockMode::None || Whitelist.Contains(InKeyEvent.GetKey())) return false;

    if (CurrentMode == EInputBlockMode::OnlyUI)
    {
        // Korrekt: Fokus über User 0 abfragen
        TSharedPtr<SWidget> FocusedWidget = SlateApp.GetUserFocusedWidget(0);
        if (FocusedWidget.IsValid() && FocusedWidget->GetTypeAsString() != TEXT("SGameLayerManager")) return false;
    }
    return true;
}

bool FGlobalInputProcessor::HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent)
{
    if (InKeyEvent.GetKey() == KeyToTrack)
    {
        bIsTrackedKeyDown = false;
        CurrentHoldTime = 0.0f;
    }
    return false;
}

bool FGlobalInputProcessor::HandleMouseButtonDownEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent)
{
    if (CurrentMode == EInputBlockMode::None) return false;
    if (CurrentMode == EInputBlockMode::TotalBlock) return true;

    // Korrekt: Hittest über Window-Location
    FWidgetPath WidgetsUnderCursor = SlateApp.LocateWindowUnderMouse(MouseEvent.GetScreenSpacePosition(), SlateApp.GetInteractiveTopLevelWindows());
    if (WidgetsUnderCursor.IsValid())
    {
        FString WidgetType = WidgetsUnderCursor.GetLastWidget()->GetTypeAsString();
        if (WidgetType != TEXT("SGameLayerManager") && WidgetType != TEXT("SViewport")) return false;
    }
    return (CurrentMode != EInputBlockMode::Cinematic);
}

bool FGlobalInputProcessor::HandleAnalogInputEvent(FSlateApplication& SlateApp, const FAnalogInputEvent& InAnalogInputEvent)
{
    return (CurrentMode != EInputBlockMode::None && !Whitelist.Contains(InAnalogInputEvent.GetKey()));
}

bool FGlobalInputProcessor::HandleMouseMoveEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent)
{
    return (CurrentMode == EInputBlockMode::TotalBlock);
}

void FGlobalInputProcessor::Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor)
{
    if (bIsTrackedKeyDown && CurrentMode != EInputBlockMode::None && KeyToTrack != EKeys::Invalid)
    {
        CurrentHoldTime += DeltaTime;
        if (CurrentHoldTime >= RequiredHoldTime)
        {
            if (OnHoldActionCompleted) OnHoldActionCompleted();
            CurrentHoldTime = 0.0f;
            bIsTrackedKeyDown = false;
        }
    }
}

// --- SUBSYSTEM IMPLEMENTATION ---

void UOBUserInputSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Processor = MakeShared<FGlobalInputProcessor>();
    Processor->Whitelist.Add(EKeys::Escape);
    Processor->Whitelist.Add(EKeys::Tilde);

    if (FSlateApplication::IsInitialized())
    {
        FSlateApplication::Get().RegisterInputPreProcessor(Processor);
    }
}

void UOBUserInputSubsystem::Deinitialize()
{
    if (FSlateApplication::IsInitialized() && Processor.IsValid())
    {
        FSlateApplication::Get().UnregisterInputPreProcessor(Processor);
    }
    Processor.Reset();
}

void UOBUserInputSubsystem::SetInputBlockMode(EInputBlockMode NewMode)
{
    if (Processor.IsValid())
    {
        Processor->CurrentMode = NewMode;
        OnInputBlockModeChanged.Broadcast(NewMode);
    }
}

void UOBUserInputSubsystem::SetupHoldToSkip(FKey Key, float Duration)
{
    if (Processor.IsValid())
    {
        Processor->KeyToTrack = Key;
        Processor->RequiredHoldTime = Duration;
        Processor->OnHoldActionCompleted = [this]() { OnHoldActionComplete.Broadcast(); };
    }
}

float UOBUserInputSubsystem::GetHoldProgress() const
{
    return (Processor.IsValid() && Processor->RequiredHoldTime > 0.0f) ? FMath::Clamp(Processor->CurrentHoldTime / Processor->RequiredHoldTime, 0.0f, 1.0f) : 0.0f;
}