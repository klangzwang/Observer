#pragma once
#include "GameFramework/GameMode.h"
#include "OBTypes.h"
#include "GameplayTagContainer.h"
#include "OBGameMode.generated.h"

class UCommonActivatableWidget;
class AOBCharacter;
class UObject;

UCLASS(MinimalAPI, Blueprintable)
class AOBGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:

	AOBGameModeBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};

UCLASS(MinimalAPI, Blueprintable)
class AOBGameMode : public AGameMode
{
	GENERATED_BODY()

public:

	AOBGameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void InitGameState() override;

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Classes, meta = (AllowPrivateAccess = true))
	FOBHUDLayoutRequest HUDLayout;
};
