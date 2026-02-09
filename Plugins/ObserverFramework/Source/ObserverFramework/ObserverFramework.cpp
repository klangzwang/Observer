#include "ObserverFramework.h"
#include "OBWorldSettings.h"
#include "AssetRegistry/AssetRegistryModule.h"

#define LOCTEXT_NAMESPACE "FObserverFrameworkModule"

void FObserverFrameworkModule::StartupModule()
{
}

void FObserverFrameworkModule::ShutdownModule()
{
}

UMeshComponent* CreateCustomDepthOutlineMesh(UMeshComponent* Archetype, AActor* Owner)
{
	UMeshComponent* CustomDepthMesh = DuplicateObject<UMeshComponent>(Archetype, Owner);
	CustomDepthMesh->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	if (Cast<USkeletalMeshComponent>(CustomDepthMesh) != nullptr)
	{
	}
	CustomDepthMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CustomDepthMesh->SetSimulatePhysics(false);
	CustomDepthMesh->SetCastShadow(false);
	if (Cast<USkinnedMeshComponent>(CustomDepthMesh) != nullptr)
	{
		((USkinnedMeshComponent*)CustomDepthMesh)->SetLeaderPoseComponent((USkinnedMeshComponent*)Archetype);
	}
	for (int32 i = 0; i < CustomDepthMesh->GetNumMaterials(); i++)
	{
		CustomDepthMesh->SetMaterial(i, UMaterial::GetDefaultMaterial(MD_Surface));
	}
	CustomDepthMesh->BoundsScale = 15000.f;
	CustomDepthMesh->SetVisibility(true);
	CustomDepthMesh->bHiddenInGame = false;
	CustomDepthMesh->bRenderInMainPass = false;
	CustomDepthMesh->bRenderCustomDepth = true;
	CustomDepthMesh->AttachToComponent(Archetype, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	CustomDepthMesh->SetRelativeLocation(FVector::ZeroVector);
	CustomDepthMesh->SetRelativeRotation(FRotator::ZeroRotator);
	CustomDepthMesh->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	return CustomDepthMesh;
}

void SetTimerUFunc(UObject* Obj, FName FuncName, float Time, bool bLooping)
{
	if (Obj != NULL)
	{
		const UWorld* const World = GEngine->GetWorldFromContextObject(Obj, EGetWorldErrorMode::Assert);
		if (World != NULL)
		{
			UFunction* const Func = Obj->FindFunction(FuncName);
			if (Func == NULL)
			{
				UE_LOG(LogTemp, Warning, TEXT("SetTimer: Object %s does not have a function named '%s'"), *Obj->GetName(), *FuncName.ToString());
			}
			else if (Func->ParmsSize > 0)
			{
				// User passed in a valid function, but one that takes parameters
				// FTimerDynamicDelegate expects zero parameters and will choke on execution if it tries
				// to execute a mismatched function
				UE_LOG(LogTemp, Warning, TEXT("SetTimer passed a function (%s) that expects parameters."), *FuncName.ToString());
			}
			else
			{
				FTimerDynamicDelegate Delegate;
				Delegate.BindUFunction(Obj, FuncName);

				FTimerHandle Handle = World->GetTimerManager().K2_FindDynamicTimerHandle(Delegate);
				World->GetTimerManager().SetTimer(Handle, Delegate, Time, bLooping);
			}
		}
	}
}
bool IsTimerActiveUFunc(UObject* Obj, FName FuncName, float* TotalTime, float* ElapsedTime)
{
	if (Obj != NULL)
	{
		const UWorld* const World = GEngine->GetWorldFromContextObject(Obj, EGetWorldErrorMode::Assert);
		if (World != NULL)
		{
			UFunction* const Func = Obj->FindFunction(FuncName);
			if (Func == NULL)
			{
				UE_LOG(LogTemp, Warning, TEXT("IsTimerActive: Object %s does not have a function named '%s'"), *Obj->GetName(), *FuncName.ToString());
				return false;
			}
			else
			{
				FTimerDynamicDelegate Delegate;
				Delegate.BindUFunction(Obj, FuncName);

				FTimerHandle Handle = World->GetTimerManager().K2_FindDynamicTimerHandle(Delegate);
				if (World->GetTimerManager().IsTimerActive(Handle))
				{
					if (TotalTime != NULL)
					{
						*TotalTime = World->GetTimerManager().GetTimerRate(Handle);
					}
					if (ElapsedTime != NULL)
					{
						*ElapsedTime = World->GetTimerManager().GetTimerElapsed(Handle);
					}
					return true;
				}
				else
				{
					return false;
				}
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}
void ClearTimerUFunc(UObject* Obj, FName FuncName)
{
	if (Obj != NULL)
	{
		const UWorld* const World = GEngine->GetWorldFromContextObject(Obj, EGetWorldErrorMode::Assert);
		if (World != NULL)
		{
			UFunction* const Func = Obj->FindFunction(FuncName);
			if (Func == NULL)
			{
				UE_LOG(LogTemp, Warning, TEXT("ClearTimer: Object %s does not have a function named '%s'"), *Obj->GetName(), *FuncName.ToString());
			}
			else
			{
				FTimerDynamicDelegate Delegate;
				Delegate.BindUFunction(Obj, FuncName);

				FTimerHandle Handle = World->GetTimerManager().K2_FindDynamicTimerHandle(Delegate);
				return World->GetTimerManager().ClearTimer(Handle);
			}
		}
	}
}

void GetAllAssetData(UClass* BaseClass, TArray<FAssetData>& AssetList, bool bRequireEntitlements)
{
	// calling this with UBlueprint::StaticClass() is probably a bug where the user intended to call GetAllBlueprintAssetData()
	if (BaseClass == UBlueprint::StaticClass())
	{
#if DO_GUARD_SLOW
		UE_LOG(UT, Fatal, TEXT("GetAllAssetData() should not be used for blueprints; call GetAllBlueprintAssetData() instead"));
#else
		UE_LOG(LogTemp, Error, TEXT("GetAllAssetData() should not be used for blueprints; call GetAllBlueprintAssetData() instead"));
#endif
		return;
	}

	// force disable local entitlement checks on dedicated server
	bRequireEntitlements = bRequireEntitlements && !IsRunningDedicatedServer();

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	TArray<FString> RootPaths;
	FPackageName::QueryRootContentPaths(RootPaths);

#if WITH_EDITOR
	// HACK: workaround for terrible registry performance when scanning; limit search paths to improve perf a bit
	RootPaths.Remove(TEXT("/Engine/"));
	RootPaths.Remove(TEXT("/Game/"));
	RootPaths.Remove(TEXT("/Paper2D/"));
	RootPaths.Add(TEXT("/Game/RestrictedAssets/Maps/"));
	RootPaths.Add(TEXT("/Game/Maps/"));
	RootPaths.Add(TEXT("/Game/RestrictedAssets/Blueprints/"));
	RootPaths.Add(TEXT("/Game/RestrictedAssets/Pickups/"));
	RootPaths.Add(TEXT("/Game/RestrictedAssets/Weapons/"));
	RootPaths.Add(TEXT("/Game/RestrictedAssets/Character/"));
	RootPaths.Add(TEXT("/Game/RestrictedAssets/ProfileItems/"));
	RootPaths.Add(TEXT("/Game/EpicInternal/Lea/"));
	RootPaths.Add(TEXT("/Game/EpicInternal/Batrankus/"));
	RootPaths.Add(TEXT("/Game/EpicInternal/Backspace/"));
	RootPaths.Add(TEXT("/Game/EpicInternal/Polaris/"));
	RootPaths.Add(TEXT("/Game/EpicInternal/Unsaved/"));
	RootPaths.Add(TEXT("/Game/EpicInternal/Salt/"));
	RootPaths.Add(TEXT("/Game/EpicInternal/Pistola/"));
	RootPaths.Add(TEXT("/Game/EpicInternal/Stu/"));
	RootPaths.Add(TEXT("/Game/EpicInternal/SR/"));
	RootPaths.Add(TEXT("/Game/EpicInternal/Loot/"));
	// Cooked data has the asset data already set up
	AssetRegistry.ScanPathsSynchronous(RootPaths);
#endif

	FARFilter ARFilter;
	if (BaseClass != NULL)
	{
		ARFilter.ClassNames.Add(BaseClass->GetFName());
		// Add any old names to the list in case things haven't been resaved
		TArray<FName> OldNames = FLinkerLoad::FindPreviousNamesForClass(BaseClass->GetPathName(), false);
		ARFilter.ClassNames.Append(OldNames);
	}
	ARFilter.bRecursivePaths = true;
	ARFilter.bIncludeOnlyOnDiskAssets = true;
	ARFilter.bRecursiveClasses = true;

	AssetRegistry.GetAssets(ARFilter, AssetList);

	// query entitlements for any assets and remove those that are not usable
	/*
	if (bRequireEntitlements)
	{
		for (int32 i = AssetList.Num() - 1; i >= 0; i--)
		{
			if (!LocallyHasEntitlement(GetRequiredEntitlementFromAsset(AssetList[i])))
			{
				AssetList.RemoveAt(i);
			}
			else
			{
				const FString* ReqAchievement = AssetList[i].TagsAndValues.Find(FName(TEXT("RequiredAchievement")));
				if (ReqAchievement != NULL && !LocallyHasAchievement(**ReqAchievement))
				{
					UE_LOG(LogTemp, Verbose, TEXT("Don't have achievement for %s"), *AssetList[i].AssetName.ToString());
					AssetList.RemoveAt(i);
				}
				else
				{
					const FString* NeedsItem = AssetList[i].TagsAndValues.Find(FName(TEXT("bRequiresItem")));
					if (NeedsItem != NULL && NeedsItem->ToBool() && !LocallyOwnsItemFor(AssetList[i].ObjectPath.ToString()))
					{
						AssetList.RemoveAt(i);
					}
				}
			}
		}
	}
	*/
}

void GetAllBlueprintAssetData(UClass* BaseClass, TArray<FAssetData>& AssetList, bool bRequireEntitlements)
{
	// force disable local entitlement checks on dedicated server
	bRequireEntitlements = bRequireEntitlements && !IsRunningDedicatedServer();

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	TArray<FString> RootPaths;
	FPackageName::QueryRootContentPaths(RootPaths);

#if WITH_EDITOR
	// HACK: workaround for terrible registry performance when scanning; limit search paths to improve perf a bit
	RootPaths.Remove(TEXT("/Engine/"));
	RootPaths.Remove(TEXT("/Game/"));
	RootPaths.Remove(TEXT("/Paper2D/"));
	RootPaths.Add(TEXT("/Game/RestrictedAssets/Maps/"));
	RootPaths.Add(TEXT("/Game/Maps/"));
	RootPaths.Add(TEXT("/Game/RestrictedAssets/Blueprints/"));
	RootPaths.Add(TEXT("/Game/RestrictedAssets/Pickups/"));
	RootPaths.Add(TEXT("/Game/RestrictedAssets/Weapons/"));
	RootPaths.Add(TEXT("/Game/RestrictedAssets/Character/"));
	RootPaths.Add(TEXT("/Game/RestrictedAssets/UI/Crosshairs/"));
	RootPaths.Add(TEXT("/Game/EpicInternal/PK/"));
	RootPaths.Add(TEXT("/Game/EpicInternal/Teams/"));
	RootPaths.Add(TEXT("/Game/EpicInternal/Cosmetic_Items/"));
	RootPaths.Add(TEXT("/Game/EpicInternal/WeaponSkins/"));
	// Cooked data has the asset data already set up
	AssetRegistry.ScanPathsSynchronous(RootPaths);
#endif

	FARFilter ARFilter;
	ARFilter.ClassNames.Add(UBlueprint::StaticClass()->GetFName());

	/*for (int32 PathIndex = 0; PathIndex < RootPaths.Num(); PathIndex++)
	{
		ARFilter.PackagePaths.Add(FName(*RootPaths[PathIndex]));
	}*/

	ARFilter.bRecursivePaths = true;
	ARFilter.bRecursiveClasses = true;
	ARFilter.bIncludeOnlyOnDiskAssets = true;

	if (BaseClass == NULL)
	{
		AssetRegistry.GetAssets(ARFilter, AssetList);
	}
	else
	{
		// TODO: the below filtering is torturous because the asset registry does not contain full inheritance information for blueprints
		// nor does it return full class paths when you request a class tree

		/*

		TArray<FAssetData> LocalAssetList;
		AssetRegistry.GetAssets(ARFilter, LocalAssetList);

		TSet<FString> UnloadedBaseClassPaths;
		// first pass: determine the inheritance that we can trivially verify are the correct class because their parent is in memory
		for (int32 i = 0; i < LocalAssetList.Num(); i++)
		{
			const FString* LoadedParentClass = LocalAssetList[i].TagsAndValues.FindTag("ParentClass");
			if (LoadedParentClass != NULL && !LoadedParentClass->IsEmpty())
			{
				UClass* Class = FindObject<UClass>(ANY_PACKAGE, **LoadedParentClass);
				if (Class == NULL)
				{
					// apparently you have to 'load' native classes once for FindObject() to reach them
					// figure out if this parent is such a class and if so, allow LoadObject()
					FString ParentPackage = *LoadedParentClass;
					ConstructorHelpers::StripObjectClass(ParentPackage);
					if (ParentPackage.StartsWith(TEXT("/Script/")))
					{
						ParentPackage = ParentPackage.LeftChop(ParentPackage.Len() - ParentPackage.Find(TEXT(".")));
						if (FindObject<UPackage>(NULL, *ParentPackage) != NULL)
						{
							Class = LoadObject<UClass>(NULL, **LoadedParentClass, NULL, LOAD_NoWarn | LOAD_Quiet);
						}
					}
				}
				if (Class != NULL)
				{
					if (Class->IsChildOf(BaseClass))
					{
						AssetList.Add(LocalAssetList[i]);
						const FString* GenClassPath = LocalAssetList[i].TagsAndValues.FindTag("GeneratedClass");
						if (GenClassPath != NULL)
						{
							UnloadedBaseClassPaths.Add(*GenClassPath);
						}
					}
					LocalAssetList.RemoveAt(i);
					i--;
				}
			}
			else
			{
				// asset info is missing; fail
				LocalAssetList.RemoveAt(i);
				i--;
			}
		}
		// now go through the remainder and match blueprints against an unloaded parent
		// if we find no new matching assets, the rest must be the wrong super
		bool bFoundAny = false;
		do
		{
			bFoundAny = false;
			for (int32 i = 0; i < LocalAssetList.Num(); i++)
			{
				if (UnloadedBaseClassPaths.Find(*LocalAssetList[i].TagsAndValues.FindTag("ParentClass")))
				{
					AssetList.Add(LocalAssetList[i]);
					const FString* GenClassPath = LocalAssetList[i].TagsAndValues.FindTag("GeneratedClass");
					if (GenClassPath != NULL)
					{
						UnloadedBaseClassPaths.Add(*GenClassPath);
					}
					LocalAssetList.RemoveAt(i);
					i--;
					bFoundAny = true;
				}
			}
		} while (bFoundAny && LocalAssetList.Num() > 0);

		*/
	}

	// query entitlements for any assets and remove those that are not usable
	/*
	if (bRequireEntitlements)
	{
		for (int32 i = AssetList.Num() - 1; i >= 0; i--)
		{
			if (!LocallyHasEntitlement(GetRequiredEntitlementFromAsset(AssetList[i])))
			{
				AssetList.RemoveAt(i);
			}
			else
			{
				const FString* ReqAchievement = AssetList[i].TagsAndValues.Find(FName(TEXT("RequiredAchievement")));
				if (ReqAchievement != NULL && !LocallyHasAchievement(**ReqAchievement))
				{
					UE_LOG(LogTemp, Verbose, TEXT("Don't have achievement for %s"), *AssetList[i].AssetName.ToString());
					AssetList.RemoveAt(i);
				}
				else
				{
					const FString* NeedsItem = AssetList[i].TagsAndValues.Find(FName(TEXT("bRequiresItem")));
					if (NeedsItem != NULL && NeedsItem->ToBool() && !LocallyOwnsItemFor(AssetList[i].ObjectPath.ToString()))
					{
						AssetList.RemoveAt(i);
					}
				}
			}
		}
	}
	*/
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FObserverFrameworkModule, ObserverFramework)
