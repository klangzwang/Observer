#pragma once
#include "Engine/GameEngine.h"
#include "OBGameEngine.generated.h"

class IEngineLoop;
class UObject;

UCLASS()
class UOBGameEngine : public UGameEngine
{
	GENERATED_BODY()

public:

	UOBGameEngine(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	virtual void Init(IEngineLoop* InEngineLoop) override;
	
public:

	/** default screenshot used for levels when none provided in the level itself */
	UPROPERTY()
	UTexture2D* DefaultLevelScreenshot;
	
	UPROPERTY()
	FText ReadEULACaption;
	UPROPERTY()
	FText ReadEULAText;
	
	/** used to display EULA info on first run */
	UPROPERTY(globalconfig)
	bool bFirstRun;
	
	virtual void SetupLoadingScreen();

protected:

	void OnLoadingMoviePlaybackFinished();

public:

	static FText ConvertTime(FText Prefix, FText Suffix, int32 Seconds, bool bForceHours = true, bool bForceMinutes = true, bool bForceTwoDigits = true)
	{
		int32 Hours = Seconds / 3600;
		Seconds -= Hours * 3600;
		int32 Mins = Seconds / 60;
		Seconds -= Mins * 60;
		bool bDisplayHours = bForceHours || Hours > 0;
		bool bDisplayMinutes = bDisplayHours || bForceMinutes || Mins > 0;

		FFormatNamedArguments Args;
		FNumberFormattingOptions Options;

		Options.MinimumIntegralDigits = 2;
		Options.MaximumIntegralDigits = 2;

		Args.Add(TEXT("Hours"), FText::AsNumber(Hours, bForceTwoDigits ? &Options : NULL));
		Args.Add(TEXT("Minutes"), FText::AsNumber(Mins, (bDisplayHours || bForceTwoDigits) ? &Options : NULL));
		Args.Add(TEXT("Seconds"), FText::AsNumber(Seconds, (bDisplayMinutes || bForceTwoDigits) ? &Options : NULL));
		Args.Add(TEXT("Prefix"), Prefix);
		Args.Add(TEXT("Suffix"), Suffix);

		if (bDisplayHours)
		{
			return FText::Format(NSLOCTEXT("UTGameEngine", "ConvertTimeHours", "{Prefix}{Hours}:{Minutes}:{Seconds}{Suffix}"), Args);
		}
		else if (bDisplayMinutes)
		{
			return FText::Format(NSLOCTEXT("UTGameEngine", "ConvertTimeMinutes", "{Prefix}{Minutes}:{Seconds}{Suffix}"), Args);
		}
		else
		{
			return FText::Format(NSLOCTEXT("UTGameEngine", "ConvertTime", "{Prefix}{Seconds}{Suffix}"), Args);
		}
	}
};
