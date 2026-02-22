#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "IOBInteractable.h"
#include "OBtypes.h"
#include "OBInteractionTargetComponent.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionSignature, AActor*, Interactor);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionBegin, APawn*, InteractorPawn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInteractionEnd, APawn*, InteractorPawn, EInteractionResult, Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnInteractionUpdated, float, Alpha, int32, Repeated, APawn*, InteractorPawn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionReactivated, APawn*, ForPawn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteractionDeactivated);

UCLASS(Blueprintable, ClassGroup = Custom, meta = (BlueprintSpawnableComponent))
class UOBInteractionTargetComponent : public UActorComponent
{
    GENERATED_BODY()

public:    

    UOBInteractionTargetComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Observer|Config")
    FInteractionData InteractionConfig;

    //UPROPERTY(BlueprintAssignable, Category = "Observer|Config")
    //FOnInteractionSignature OnInteractTriggered;

    UPROPERTY(BlueprintAssignable, Category = "Observer|Config")
    FOnInteractionBegin OnInteractionBeginDelegate;

    UPROPERTY(BlueprintAssignable, Category = "Observer|Config")
    FOnInteractionEnd OnInteractionEndDelegate;

    UPROPERTY(BlueprintAssignable, Category = "Observer|Config")
    FOnInteractionUpdated OnInteractionUpdatedDelegate;

    UPROPERTY(BlueprintAssignable, Category = "Observer|Config")
    FOnInteractionReactivated OnInteractionReactivatedDelegate;

    UPROPERTY(BlueprintAssignable, Category = "Observer|Config")
    FOnInteractionDeactivated OnInteractionDeactivatedDelegate;

protected:

    UFUNCTION(BlueprintCallable, Category = "Observer|Interaction")
    void OnInteractionBegin(APawn* InInteractorPawn);

    UFUNCTION(BlueprintCallable, Category = "Observer|Interaction")
    void OnInteractionEnd(APawn* InInteractorPawn, EInteractionResult InResult);

/*
protected:

    UFUNCTION(BlueprintCallable, Category = "Observer|Interaction")
    void TriggerInteraction(AActor* Interactor);
*/

//
// OuterInner SphereCollision
//
protected:

    UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Observer|Interaction")
    EInteractionColliderArt ColliderArt;

    UFUNCTION(BlueprintCallable, Category = "Observer|Interaction")
    void ConstructOverlapZones();

    UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Observer|Interaction")
    UPrimitiveComponent* OuterZone;

    UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Observer|Interaction")
    UPrimitiveComponent* InnerZone;

    UFUNCTION()
    void OnOuterBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnOuterEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
 
    UFUNCTION()
    void OnInnerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
	void OnInnerEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

//
//
//
protected:

    UPROPERTY()
    class UWidgetComponent* MarkerWidgetComp;

    UPROPERTY(EditAnywhere, Category = "Observer|InteractionUI")
    float DetectionRadius = 300.0f;

public:

    void ShowMarker();
    void HideMarker();

/*
    UFUNCTION()
    void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
*/
};