#pragma once
#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FObserverFrameworkModule : public IModuleInterface
{
	
public:

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

template <typename ENUM>
static ENUM ConvertStringToEnum(const FString& EnumStringValue)
{
	int32 FoundValue = 0;
	
	UEnum* EnumClass = StaticEnum<ENUM>();
	
	for (int32 i=0; i<EnumClass->NumEnums(); i++)
	{
		if (EnumClass->GetNameStringByIndex(i) == EnumStringValue)
		{
			FoundValue = i;
			break;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("bla bla bla %d"), FoundValue);
	
	return static_cast<ENUM>(FoundValue);
}
