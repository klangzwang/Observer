#pragma once
#include "CoreMinimal.h"
#include "Engine/TimerHandle.h"
#include "OBSkeletalMeshComponent.generated.h"

class AOBCharacter;
class UAnimInstance;

USTRUCT(BlueprintType)
struct FAnimLayerEntry
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
	TSubclassOf<UAnimInstance> Layer;
};

USTRUCT(BlueprintType)
struct FDynamicMeshSet
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
	TArray<USkeletalMesh*> SkeletalMeshes;
};

USTRUCT(BlueprintType)
struct FCosmeticSet
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
	TArray<FDynamicMeshSet> MeshSets;
};

UCLASS(Blueprintable, EditInlineNew, ClassGroup = Custom, meta = (BlueprintSpawnableComponent))
class UOBSkeletalMeshComponent : public USkeletalMeshComponent {

    GENERATED_BODY()

public:

    UOBSkeletalMeshComponent(const FObjectInitializer& ObjectInitializer);

protected:

    virtual void BeginPlay() override;

public:

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blinking")
    float MaxBlinkInterval = 5.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blinking")
    float MinBlinkInterval = 2.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blinking")
    float BlinkDuration = 0.2f;

private:

    FTimerHandle BlinkTimerHandle;

    UFUNCTION()
    void StartBlink();

    void AnimateBlink(float DeltaTime);

    float BlinkAlpha;
    bool bIsBlinking;
    float BlinkRate;

private:

    UPROPERTY()
    USkeletalMeshComponent* SkeletalMeshComp;
	
protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Observer|AnimLayer", meta = (AllowPrivateAccess = true))
	FAnimLayerEntry AnimLayer;
	
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Observer|Cosmetics")
	FCosmeticSet CosmeticSets;

public:

	UFUNCTION(BlueprintCallable, Category = "Observer|DynamicMeshes")
	void ConstructDynamicMesh();
};
