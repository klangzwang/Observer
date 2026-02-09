#pragma once
#include "GameFramework/WorldSettings.h"
#include "Engine/DataTable.h"
#include "OBWorldSettings.generated.h"

USTRUCT(BlueprintType)
struct FWorldSetup : public FTableRowBase
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
	bool PlayIntroFadeBlack = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
	bool PlayOutroFadeBlack = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
	float IntroFadeBlackTime = 24.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
	float OutroFadeBlackTime = 24.f;

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
	bool PlayIntroCamSequence = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
	bool PlayOutroCamSequence = false;
};

UCLASS()
class OBSERVERFRAMEWORK_API AOBWorldSettings : public AWorldSettings
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category = World)
	static AOBWorldSettings* GetWorldSettings(UObject* WorldContextObject);

public:

	AOBWorldSettings(const FObjectInitializer& ObjectInitializer);

	virtual void NotifyBeginPlay() override;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool DoSetup = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (EditCondition = "DoSetup"))
	FWorldSetup Setup;

public:

	UFUNCTION(BlueprintCallable, Category = "Observer|WorldSettings")
	FWorldSetup GetWorldSetup() { return Setup; };

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = LevelSettings)
	USoundBase* Music;

	UPROPERTY()
	UAudioComponent* MusicComp;
};
