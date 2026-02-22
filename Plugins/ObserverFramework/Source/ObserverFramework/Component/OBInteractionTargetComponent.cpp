#include "OBInteractionTargetComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"

UOBInteractionTargetComponent::UOBInteractionTargetComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ColliderArt = EInteractionColliderArt::Sphere;

    OnInteractionBeginDelegate.AddDynamic(this, &UOBInteractionTargetComponent::OnInteractionBegin);
    OnInteractionEndDelegate.AddDynamic(this, &UOBInteractionTargetComponent::OnInteractionEnd);
}

void UOBInteractionTargetComponent::BeginPlay()
{
    Super::BeginPlay();

    AActor* Owner = GetOwner();
    if (!Owner)
        return;

    ConstructOverlapZones();

    MarkerWidgetComp = Owner->FindComponentByClass<UWidgetComponent>();
    if (MarkerWidgetComp)
    {
        MarkerWidgetComp->SetVisibility(false);
        MarkerWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
        MarkerWidgetComp->SetDrawAtDesiredSize(true);
    }

    /*
    USphereComponent* TriggerSphere = Owner->FindComponentByClass<USphereComponent>();
    if (TriggerSphere)
    {
        TriggerSphere->SetSphereRadius(DetectionRadius);
        TriggerSphere->OnComponentBeginOverlap.AddDynamic(this, &UOBInteractionTargetComponent::OnSphereBeginOverlap);
        TriggerSphere->OnComponentEndOverlap.AddDynamic(this, &UOBInteractionTargetComponent::OnSphereEndOverlap);
    }
	*/
}

void UOBInteractionTargetComponent::OnInteractionBegin(APawn* InInteractorPawn)
{
    OnInteractionBeginDelegate.Broadcast(InInteractorPawn);
}

void UOBInteractionTargetComponent::OnInteractionEnd(APawn* InInteractorPawn, EInteractionResult InResult)
{

}

void UOBInteractionTargetComponent::ConstructOverlapZones()
{
    if (ColliderArt == EInteractionColliderArt::Sphere)
    {
        USphereComponent* OuterSphere = NewObject<USphereComponent>(GetOwner(), TEXT("OuterZone"));
        OuterSphere->SetSphereRadius(200.f, true);
        USphereComponent* InnerSphere = NewObject<USphereComponent>(GetOwner(), TEXT("InnerZone"));
        InnerSphere->SetSphereRadius(200.f, true);
        OuterZone = OuterSphere;
        InnerZone = InnerSphere;
    }
    else
    {
        UBoxComponent* OuterBox = NewObject<UBoxComponent>(GetOwner(), TEXT("OuterZone"));
        OuterBox->SetBoxExtent(FVector(64.f, 64.f, 64.f), true);
        UBoxComponent* InnerBox = NewObject<UBoxComponent>(GetOwner(), TEXT("InnerZone"));
        InnerBox->SetBoxExtent(FVector(64.f, 64.f, 64.f), true);
        OuterZone = OuterBox;
        InnerZone = InnerBox;
    }

    if (OuterZone && InnerZone)
    {
        OuterZone->OnComponentBeginOverlap.AddDynamic(this, &UOBInteractionTargetComponent::OnOuterBeginOverlap);
        OuterZone->OnComponentEndOverlap.AddDynamic(this, &UOBInteractionTargetComponent::OnOuterEndOverlap);

        InnerZone->OnComponentBeginOverlap.AddDynamic(this, &UOBInteractionTargetComponent::OnInnerBeginOverlap);
        InnerZone->OnComponentEndOverlap.AddDynamic(this, &UOBInteractionTargetComponent::OnInnerEndOverlap);

		OuterZone->SetHiddenInGame(false);   
        InnerZone->SetHiddenInGame(false);
    }

	// TODO: Setup the zones (attach to root, set collision, etc.)
}

void UOBInteractionTargetComponent::OnOuterBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor->IsA(ACharacter::StaticClass()))
    {
        ShowMarker();
    }
}

void UOBInteractionTargetComponent::OnOuterEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor->IsA(ACharacter::StaticClass()))
    {
        HideMarker();
    }
}

void UOBInteractionTargetComponent::OnInnerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}

void UOBInteractionTargetComponent::OnInnerEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

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

/*
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

void UOBInteractionTargetComponent::TriggerInteraction(AActor* Interactor)
{
    OnInteractTriggered.Broadcast(Interactor);
}
*/
