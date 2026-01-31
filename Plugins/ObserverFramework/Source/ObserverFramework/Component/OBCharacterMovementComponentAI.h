#pragma once
#include "GameFramework/CharacterMovementComponent.h"
#include "OBCharacterMovementComponentAI.generated.h"

UCLASS(Blueprintable, ClassGroup=Custom, meta=(BlueprintSpawnableComponent))
class OBSERVERFRAMEWORK_API UOBCharacterMovementComponentAI : public UCharacterMovementComponent
{
    GENERATED_BODY()
	
public:

    UOBCharacterMovementComponentAI(const FObjectInitializer& ObjectInitializer);

public:

    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
