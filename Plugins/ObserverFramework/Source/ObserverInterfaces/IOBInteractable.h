#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IOBInteractable.generated.h"

USTRUCT(BlueprintType)
struct FInteractionData
{
    GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
    FString ActionName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
    float InteractionDuration;
};

UINTERFACE(MinimalAPI)
class UOBInteractable : public UInterface
{
    GENERATED_BODY()
};

class OBSERVERINTERFACES_API IOBInteractable
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
    void OnInteract(AActor* Caller);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
    FInteractionData GetInteractionData();
};