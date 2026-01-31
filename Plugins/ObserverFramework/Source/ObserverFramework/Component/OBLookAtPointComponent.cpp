#include "OBLookAtPointComponent.h"
#include "Engine/TargetPoint.h"
#include "Engine/World.h"

UOBLookAtPointComponent::UOBLookAtPointComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

ATargetPoint* UOBLookAtPointComponent::GetTargetPoint()
{
	if (!targetPoint)
	{
		FActorSpawnParameters spawnParam;
		spawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		targetPoint = GetWorld()->SpawnActor<ATargetPoint>(GetComponentLocation(), GetComponentRotation(), spawnParam);
		targetPoint->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	}

	return targetPoint;
}
