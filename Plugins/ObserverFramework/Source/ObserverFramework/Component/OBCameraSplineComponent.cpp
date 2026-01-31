#include "OBCameraSplineComponent.h"
#include "OBLookAtPointComponent.h"
#include "GameFramework/Actor.h"

AActor* UOBCameraSplineComponent::GetSequenceLookAtPointLocation() const
{
	return GetLookAtPointLocationByName(SequenceSettings.LookAtPoint);
}

AActor* UOBCameraSplineComponent::GetLookAtPointLocationByName(const FName& lookAtPointName) const
{
	TArray<UOBLookAtPointComponent*> lookAtPoints;
	GetOwner()->GetComponents<UOBLookAtPointComponent>(lookAtPoints, true);

	for (auto point : lookAtPoints)
	{
		if (point->GetPointName() == lookAtPointName)
			return (AActor*)(point->GetTargetPoint());
	}

	return GetOwner();
}
