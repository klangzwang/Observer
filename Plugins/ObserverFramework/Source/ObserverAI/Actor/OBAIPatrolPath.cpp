#include "OBAIPatrolPath.h"
#include "Kismet/KismetSystemLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OBAIPatrolPath)

AOBAIPatrolPath::AOBAIPatrolPath()
{
	PrimaryActorTick.bCanEverTick = false;

	SplineComponent = CreateDefaultSubobject<UOBAISplineComponent>(TEXT("Spline"));

    FVector Origin;
    FVector BoxExtent;
    float SphereRadius;

    UKismetSystemLibrary::GetComponentBounds(
        SplineComponent,
        Origin,
        BoxExtent,
        SphereRadius
    );

#if WITH_EDITORONLY_DATA
    SplineComponent->GetIcon()->SetRelativeLocation(Origin);
#endif
}

void AOBAIPatrolPath::BeginPlay()
{
	Super::BeginPlay();
}

int32 AOBAIPatrolPath::GetSplinePointNum() const
{
	return SplineComponent->GetNumberOfSplinePoints();
}
