#include "OBDeviceComponent.h"
#include "OBDeviceBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OBDeviceComponent)

UOBDeviceComponent::UOBDeviceComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	//->OnUpdateProgressDelegate.AddDynamic(this, &UOBDeviceComponent::OnUpdateProgress);
	//->OnUpdateSegmentDelegate.AddDynamic(this, &UOBDeviceComponent::OnUpdateSegment);
	//->OnUpdateStatusDelegate.AddDynamic(this, &UOBDeviceComponent::OnUpdateStatus);
}

AOBDeviceBase* UOBDeviceComponent::GetDeviceByName(FName InDeviceName) const
{
	for (AOBDeviceBase* Device : Devices)
	{
		if (Device && Device->GetFName() == InDeviceName)
		{
			return Device;
		}
	}
	return nullptr;
}

void UOBDeviceComponent::OnUpdateProgress(int32 InCurrentProgress)
{
	/*
	if (BulletCounterUI)
	{
		BulletCounterUI->BP_UpdateBulletCounter(MagazineSize, Bullets);
	}
	*/
}

void UOBDeviceComponent::OnUpdateSegment(int32 InCurrentSegment)
{
	/*
	if (BulletCounterUI)
	{
		BulletCounterUI->BP_UpdateBulletCounter(MagazineSize, Bullets);
	}
	*/
}

void UOBDeviceComponent::OnUpdateStatus(bool InNewStatus)
{
	/*
	if (IsValid(BulletCounterUI))
	{
		BulletCounterUI->BP_Damaged(LifePercent);
	}
	*/
}