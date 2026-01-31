#include "OBAnimInstance.h"
#include "OBCharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OBAnimInstance)

UOBAnimInstance::UOBAnimInstance()
{
    bIsHeadActive = false;
    HeadAlpha = 0.0f;
    HeadMorphAlpha = 0.0f;
    HeadRate = 0.0f;
}

void UOBAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

    float InitialHeadDelay = UKismetMathLibrary::RandomFloatInRange(MinHeadInterval, MaxHeadInterval);
    GetWorld()->GetTimerManager().SetTimer(HeadTimerHandle, this, &UOBAnimInstance::StartHead, InitialHeadDelay, false);

    StartNewMovementCycle();

    onward();
}

void UOBAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

    if (bIsHeadActive)
        AnimateHead(DeltaSeconds);

    UpdateAnimations(DeltaSeconds);
}

void UOBAnimInstance::UpdateAnimations(float DeltaTimeX)
{
    K2_UpdateAnimation(CurrentValue, HeadMorphAlpha, ticks, UKismetMathLibrary::RandomFloatInRange(0.f, 1.f));
}

/*
*
* HEAD
*
*/
void UOBAnimInstance::StartHead()
{
    if (!bIsHeadActive)
    {
        bIsHeadActive = true;
        HeadAlpha = 0.0f;
        HeadRate = 1.0f / (HeadDuration / 2.0f);

        if (UKismetMathLibrary::RandomFloatInRange(0.f, 0.1f) < 0.01f)
        {
            GetWorld()->GetTimerManager().SetTimer(HeadTimerHandle, this, &UOBAnimInstance::StartHead, 0.5f, false);
        }
        else
        {
            float NextDelay = UKismetMathLibrary::RandomFloatInRange(MinHeadInterval, MaxHeadInterval);
            GetWorld()->GetTimerManager().SetTimer(HeadTimerHandle, this, &UOBAnimInstance::StartHead, NextDelay, false);
        }
    }
}

void UOBAnimInstance::AnimateHead(float DeltaTime)
{
    HeadAlpha += DeltaTime * HeadRate;

    HeadMorphAlpha = 0.0f;
    if (HeadAlpha <= 1.0f)
    {
        HeadMorphAlpha = FMath::Lerp(0.0f, 1.0f, HeadAlpha);
    }
    else if (HeadAlpha <= 2.0f)
    {
        HeadMorphAlpha = FMath::Lerp(0.0f, 1.0f, 2.0f - HeadAlpha);
    }
    else
    {
        bIsHeadActive = false;
        HeadMorphAlpha = 0.0f;
    }
}

void UOBAnimInstance::StartNewMovementCycle()
{
    TargetValue = UKismetMathLibrary::RandomFloatInRange(MinRange, MaxRange);

    GetWorld()->GetTimerManager().SetTimer(
        MovementTimerHandle,
        this,
        &UOBAnimInstance::MoveValueToTarget,
        TimerRate,
        true
    );

    //UE_LOG(LogTemp, Warning, TEXT("New Cycle Started. Target: %f"), TargetValue);
}

void UOBAnimInstance::MoveValueToTarget()
{
    float DeltaTime = TimerRate;

    CurrentValue = FMath::FInterpTo(CurrentValue, TargetValue, DeltaTime, MovementSpeed);

    //UE_LOG(LogTemp, Log, TEXT("Current Value: %f"), CurrentValue);

    if (FMath::IsNearlyEqual(CurrentValue, TargetValue, 0.1f))
    {
        GetWorld()->GetTimerManager().ClearTimer(MovementTimerHandle);

        //UE_LOG(LogTemp, Error, TEXT("Target %f Reached! Current Value: %f"), TargetValue, CurrentValue);

        FTimerHandle RestartDelayHandle;
        GetWorld()->GetTimerManager().SetTimer(
            RestartDelayHandle,
            this,
            &UOBAnimInstance::StartNewMovementCycle,
            1.5f,
            false
        );
    }
}

void UOBAnimInstance::Pause()
{
    if (GetWorld()->GetTimerManager().IsTimerActive(TickForwardHandle))
        GetWorld()->GetTimerManager().PauseTimer(TickForwardHandle);
    else if (GetWorld()->GetTimerManager().IsTimerActive(TickBackwardHandle))
        GetWorld()->GetTimerManager().PauseTimer(TickBackwardHandle);
}

void UOBAnimInstance::Resume()
{
    if (GetWorld()->GetTimerManager().IsTimerPaused(TickForwardHandle))
        GetWorld()->GetTimerManager().UnPauseTimer(TickForwardHandle);
    else if (GetWorld()->GetTimerManager().IsTimerPaused(TickBackwardHandle))
        GetWorld()->GetTimerManager().UnPauseTimer(TickBackwardHandle);
}

void UOBAnimInstance::Stop()
{
    GetWorld()->GetTimerManager().ClearTimer(TickForwardHandle);
    GetWorld()->GetTimerManager().ClearTimer(TickBackwardHandle);
}

void UOBAnimInstance::onward()
{
    if (GetWorld()->GetTimerManager().IsTimerActive(TickBackwardHandle))
        GetWorld()->GetTimerManager().ClearTimer(TickBackwardHandle);

    GetWorld()->GetTimerManager().SetTimer(
        TickForwardHandle,
        this,
        &ThisClass::BeginOnward,
        GetWorld()->GetDeltaSeconds(),
        true,
        0.f);
}

void UOBAnimInstance::backward()
{
    if (GetWorld()->GetTimerManager().IsTimerActive(TickForwardHandle))
        GetWorld()->GetTimerManager().ClearTimer(TickForwardHandle);

    GetWorld()->GetTimerManager().SetTimer(
        TickBackwardHandle,
        this,
        &ThisClass::BeginBackward,
        GetWorld()->GetDeltaSeconds(),
        true,
        0.f);
}

void UOBAnimInstance::BeginOnward()
{
    const float DeltaTime = GetWorld()->GetTimerManager().GetTimerRate(TickForwardHandle);
    const float Step = DeltaTime / DurationInSeconds;

    Alpha = FMath::Min(Alpha + Step, 1.0f);

    if (Alpha >= 1.f)
    {
        if (PingPong)
            backward();
        else
            GetWorld()->GetTimerManager().ClearTimer(TickForwardHandle);

        ticks = 1.f;

        OnFinishedForward.Broadcast(this);
    }
    else
    {
        const float Exponent = (tickExponent > 0.0f) ? tickExponent : 1.0f;

        switch (TickBlendFunction)
        {
            case ETickerBlendFunction::Linear:
                ticks = Alpha;
                break;

            case ETickerBlendFunction::Curve:
                ticks = TickCurve.GetRichCurve()->Eval(Alpha);
                break;

            case ETickerBlendFunction::EaseIn:
                ticks = FMath::InterpEaseIn(0.0f, 1.0f, Alpha, Exponent);
                break;

            case ETickerBlendFunction::EaseOut:
                ticks = FMath::InterpEaseOut(0.0f, 1.0f, Alpha, Exponent);
                break;

            case ETickerBlendFunction::EaseInOut:
                ticks = FMath::InterpEaseInOut(0.0f, 1.0f, Alpha, Exponent);
                break;
        }

        OnTickerUpdate.Broadcast(ticks);
    }
}

void UOBAnimInstance::BeginBackward()
{
    const float DeltaTime = GetWorld()->GetTimerManager().GetTimerRate(TickBackwardHandle);
    const float Step = DeltaTime / DurationInSeconds;

    Alpha = FMath::Max(0.f, Alpha - Step);

    if (Alpha <= 0.f)
    {
        if (PingPong)
            onward();
        else
            GetWorld()->GetTimerManager().ClearTimer(TickBackwardHandle);

        ticks = 0.f;

        OnFinishedBackward.Broadcast(this);
    }
    else
    {
        const float Exponent = (tickExponent > 0.0f) ? tickExponent : 1.0f;

        switch (TickBlendFunction)
        {
            case ETickerBlendFunction::Linear:
                ticks = Alpha;
                break;

            case ETickerBlendFunction::Curve:
                ticks = TickCurve.GetRichCurve()->Eval(Alpha);
                break;

            case ETickerBlendFunction::EaseIn:
                ticks = FMath::InterpEaseIn(0.0f, 1.0f, Alpha, Exponent);
                break;

            case ETickerBlendFunction::EaseOut:
                ticks = FMath::InterpEaseOut(0.0f, 1.0f, Alpha, Exponent);
                break;

            case ETickerBlendFunction::EaseInOut:
                ticks = FMath::InterpEaseInOut(0.0f, 1.0f, Alpha, Exponent);
                break;
        }

        OnTickerUpdate.Broadcast(ticks);
    }
}
