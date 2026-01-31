#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "IOBInteractable.h"
#include "OBInteractionTargetComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionSignature, AActor*, Interactor);

UCLASS(Blueprintable, ClassGroup = Custom, meta = (BlueprintSpawnableComponent))
class UOBInteractionTargetComponent : public UActorComponent
{
    GENERATED_BODY()

public:    

    UOBInteractionTargetComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Observer|Config")
    FInteractionData InteractionConfig;

    UPROPERTY(BlueprintAssignable, Category = "Observer|Config")
    FOnInteractionSignature OnInteractTriggered;

    UFUNCTION(BlueprintCallable, Category = "Observer|Interaction")
    void TriggerInteraction(AActor* Interactor);

protected:

    UPROPERTY()
    class UWidgetComponent* MarkerWidgetComp;

    UPROPERTY(EditAnywhere, Category = "Observer|InteractionUI")
    float DetectionRadius = 300.0f;

public:

    void ShowMarker();
    void HideMarker();

    UFUNCTION()
    void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};