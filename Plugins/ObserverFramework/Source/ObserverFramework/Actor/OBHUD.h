#pragma once
#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "OBCharacterMovementComponent.h"
#include "OBHUD.generated.h"

UCLASS()
class OBSERVERFRAMEWORK_API AOBHUD : public AHUD
{
	GENERATED_BODY()

public:

	AOBHUD(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	virtual void PreInitializeComponents() override;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
