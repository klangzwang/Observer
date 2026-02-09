#include "OBGameEngine.h"
#if !UE_SERVER
#include "SlateBasics.h"
#include "MoviePlayer.h"
//#include "SUWindowsStyle.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(OBGameEngine)

class IEngineLoop;

UOBGameEngine::UOBGameEngine(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bFirstRun = true;
	ReadEULACaption = NSLOCTEXT("UTGameEngine", "ReadEULACaption", "READ ME FIRST");
	ReadEULAText = NSLOCTEXT("UTGameEngine", "ReadEULAText", "EULA TEXT");
}

void UOBGameEngine::Init(IEngineLoop* InEngineLoop)
{
	if (bFirstRun)
	{
#if !UE_SERVER
		if (GetMoviePlayer()->IsMovieCurrentlyPlaying())
		{
			// When the movie ends, ask if the user accepts the eula.  
			// Note we cannot prompt while the movie is playing due to threading issues
			GetMoviePlayer()->OnMoviePlaybackFinished().AddUObject(this, &UOBGameEngine::OnLoadingMoviePlaybackFinished);
		}
		else
#endif
		{
			// If the movie has already ended or was never played, prompt for eula now.
			//PromptForEULAAcceptance();
		}
	}

	Super::Init(InEngineLoop);
}

void UOBGameEngine::SetupLoadingScreen()
{
#if !UE_SERVER
	/*
		if (IsMoviePlayerEnabled())
		{
			FLoadingScreenAttributes LoadingScreen;
			LoadingScreen.bAutoCompleteWhenLoadingCompletes = true;
			// TODO: probably need to do something to handle aspect ratio
			//LoadingScreen.WidgetLoadingScreen = SNew(SImage).Image(&LoadingScreenImage);
			LoadingScreen.WidgetLoadingScreen = SNew(SImage).Image(SUWindowsStyle::Get().GetBrush("LoadingScreen"));
			GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
			FCoreUObjectDelegates::PreLoadMap.Broadcast(TEXT(""));
		}
	*/
#endif
}

void UOBGameEngine::OnLoadingMoviePlaybackFinished()
{

}