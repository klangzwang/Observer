#include "OBAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OBAssetManager)

UOBAssetManager::UOBAssetManager()
{
}

TSharedPtr<FStreamableHandle> UOBAssetManager::LoadPrimaryAsset(const FPrimaryAssetId& AssetToLoad, const TArray<FName>& LoadBundles, FAssetManagerLoadParams&& LoadParams, UE::FSourceLocation Location)
{
	GEngine->AddOnScreenDebugMessage(123, 4.f, FColor::White, FString::Printf(TEXT("YYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYY")));

    return Super::LoadPrimaryAsset(AssetToLoad, LoadBundles, MoveTemp(LoadParams), Location);
}
