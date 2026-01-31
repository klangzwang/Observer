#include "OBSkeletalMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"

UOBSkeletalMeshComponent::UOBSkeletalMeshComponent(const FObjectInitializer& ObjectInitializer)
 : Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = true;
    bIsBlinking = false;
    BlinkAlpha = 0.0f;
    BlinkRate = 0.0f;
}

void UOBSkeletalMeshComponent::BeginPlay()
{
    Super::BeginPlay();

    /*
    AActor* Owner = GetOwner();
    if (Owner)
    */

    TArray<TObjectPtr <USceneComponent>> SceneArray = GetAttachChildren();
    for (USceneComponent* comp : SceneArray)
    {
        if (comp->GetName().Contains("ValeaHead"))
        {
            SkeletalMeshComp = Cast<USkeletalMeshComponent>(comp);
        }
    }

    /*
    float InitialDelay = UKismetMathLibrary::RandomFloatInRange(MinBlinkInterval, MaxBlinkInterval);
    GetWorld()->GetTimerManager().SetTimer(BlinkTimerHandle, this, &UOBSkeletalMeshComponent::StartBlink, InitialDelay, false);
    */
}

void UOBSkeletalMeshComponent::StartBlink()
{
    if (!bIsBlinking)
    {
        bIsBlinking = true;
        BlinkAlpha = 0.0f;
        // Rate = 1.0 / (Halbe Blinzel-Dauer), da wir zwei Phasen haben (zu & auf)
        BlinkRate = 1.0f / (BlinkDuration / 2.0f); 

        float NextDelay = UKismetMathLibrary::RandomFloatInRange(MinBlinkInterval, MaxBlinkInterval);
        GetWorld()->GetTimerManager().SetTimer(BlinkTimerHandle, this, &UOBSkeletalMeshComponent::StartBlink, NextDelay, false);
    }
}

void UOBSkeletalMeshComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    /*
    if (bIsBlinking)
        AnimateBlink(DeltaTime);
    */
}

void UOBSkeletalMeshComponent::AnimateBlink(float DeltaTime)
{
    // Die Alpha von 0.0 -> 1.0 (Auge zu) -> 0.0 (Auge auf) bewegen.
    BlinkAlpha += DeltaTime * BlinkRate;

    float MorphWeight = 0.0f;
    if (BlinkAlpha <= 1.0f)
    {
        // Phase 1: Auge schließt (Alpha von 0.0 auf 1.0)
        // Verwenden Sie eine nicht-lineare Interpolation (z.B. Sinus oder FInterpTo/Curve), 
        // um eine natürlichere Beschleunigung/Verlangsamung der Bewegung zu erzielen.
        // Hier: Eine quadratische Funktion, die schneller schließt und langsamer öffnet 
        // (Wink-Bewegung ist oft sehr schnell)
        MorphWeight = FMath::Lerp(0.0f, 1.0f, BlinkAlpha); 
    }
    else if (BlinkAlpha <= 2.0f)
    {
        // Phase 2: Auge öffnet (Alpha von 1.0 auf 0.0)
        // Alpha muss wieder von 0.0 auf 1.0 für Lerp, daher 2.0 - BlinkAlpha
        MorphWeight = FMath::Lerp(0.0f, 1.0f, 2.0f - BlinkAlpha); 
    }
    else
    {
        // Blinzeln beendet
        bIsBlinking = false;
        MorphWeight = 0.0f;
    }
    
    SkeletalMeshComp->SetMorphTarget("blink_R", MorphWeight);
    SkeletalMeshComp->SetMorphTarget("blink_L", MorphWeight);
    SkeletalMeshComp->SetMorphTarget("squint_R", MorphWeight / 3.f);
    SkeletalMeshComp->SetMorphTarget("squint_L", MorphWeight / 3.f);
}

void UOBSkeletalMeshComponent::ConstructDynamicMesh()
{
	
}