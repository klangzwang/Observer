#pragma once
#include "Engine/AssetManager.h"
#include "OBAssetManager.generated.h"

UCLASS(Config = Game)
class OBSERVERFRAMEWORK_API UOBAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:

	UOBAssetManager();
};
