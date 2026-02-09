#pragma once
#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Engine/Canvas.h"

/** utility to create a copy of a mesh for outline effect purposes
 * the mesh is initialized but not registered
 */
extern UMeshComponent* CreateCustomDepthOutlineMesh(UMeshComponent* Archetype, AActor* Owner);

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

/** workaround for FCanvasIcon not having a constructor you can pass in the values to */
FORCEINLINE FCanvasIcon MakeCanvasIcon(UTexture* Tex, float InU, float InV, float InUL, float InVL)
{
	FCanvasIcon Result;
	Result.Texture = Tex;
	Result.U = InU;
	Result.V = InV;
	Result.UL = InUL;
	Result.VL = InVL;
	return Result;
}

/** returns asset data for all assets of the specified class
 * do not use for Blueprints as you can only query for all blueprints period; use GetAllBlueprintAssetData() to query the blueprint's underlying class
 * if bRequireEntitlements is set, assets on disk for which no local player has the required entitlement will not be returned
 *
 * WARNING: the asset registry does a class name search not a path search so the returned assets may not actually be the class you want in the case of name conflicts
 *			if you load any returned assets always verify that you got back what you were expecting!
 */
extern OBSERVERFRAMEWORK_API void GetAllAssetData(UClass* BaseClass, TArray<FAssetData>& AssetList, bool bRequireEntitlements = true);
/** returns asset data for all blueprints of the specified base class in the asset registry
 * this does not actually load assets, so it's fast in a cooked build, although the first time it is run
 * in an uncooked build it will hitch while scanning the asset registry
 * if bRequireEntitlements is set, assets on disk for which no local player has the required entitlement will not be returned
 */
extern OBSERVERFRAMEWORK_API void GetAllBlueprintAssetData(UClass* BaseClass, TArray<FAssetData>& AssetList, bool bRequireEntitlements = true);

/** timer manipulation for UFUNCTIONs that doesn't require a timer handle */
extern OBSERVERFRAMEWORK_API void SetTimerUFunc(UObject* Obj, FName FuncName, float Time, bool bLooping = false);
extern OBSERVERFRAMEWORK_API bool IsTimerActiveUFunc(UObject* Obj, FName FuncName, float* TotalTime = NULL, float* ElapsedTime = NULL);
extern OBSERVERFRAMEWORK_API void ClearTimerUFunc(UObject* Obj, FName FuncName);
