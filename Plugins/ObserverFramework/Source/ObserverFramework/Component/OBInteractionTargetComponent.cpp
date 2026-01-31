#include "OBInteractionTargetComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"

UOBInteractionTargetComponent::UOBInteractionTargetComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UOBInteractionTargetComponent::BeginPlay()
{
    Super::BeginPlay();

    AActor* Owner = GetOwner();
    if (!Owner)
        return;

    MarkerWidgetComp = Owner->FindComponentByClass<UWidgetComponent>();
    if (MarkerWidgetComp)
    {
        MarkerWidgetComp->SetVisibility(false);
        MarkerWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
        MarkerWidgetComp->SetDrawAtDesiredSize(true);
    }

    USphereComponent* TriggerSphere = Owner->FindComponentByClass<USphereComponent>();
    if (TriggerSphere)
    {
        TriggerSphere->SetSphereRadius(DetectionRadius);
        TriggerSphere->OnComponentBeginOverlap.AddDynamic(this, &UOBInteractionTargetComponent::OnSphereBeginOverlap);
        TriggerSphere->OnComponentEndOverlap.AddDynamic(this, &UOBInteractionTargetComponent::OnSphereEndOverlap);
    }
}

void UOBInteractionTargetComponent::TriggerInteraction(AActor* Interactor)
{
    OnInteractTriggered.Broadcast(Interactor);
}

void UOBInteractionTargetComponent::ShowMarker()
{
    if (MarkerWidgetComp)
        MarkerWidgetComp->SetVisibility(true);
}

void UOBInteractionTargetComponent::HideMarker()
{
    if (MarkerWidgetComp)
        MarkerWidgetComp->SetVisibility(false);
}

void UOBInteractionTargetComponent::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor->IsA(ACharacter::StaticClass()))
    {
        ShowMarker();
    }
}

void UOBInteractionTargetComponent::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor->IsA(ACharacter::StaticClass()))
    {
        HideMarker();
    }
}