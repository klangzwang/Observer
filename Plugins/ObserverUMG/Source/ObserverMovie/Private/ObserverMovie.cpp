#include "ObserverMovie.h"
//#include "GenericApplication.h"
//#include "GenericApplicationMessageHandler.h"
#include "SlateExtras.h"

class FObserverMovieModule : public IObserverMovieModule
{
	
protected:

	FSlateFontInfo LoadingFont;
	int32 count;
	
public:

	virtual void StartupModule() override
	{		
		count = 0;
#if !UE_SERVER
		FLoadingScreenAttributes LoadingScreen;
		LoadingScreen.bAutoCompleteWhenLoadingCompletes = false;
		LoadingScreen.bMoviesAreSkippable = true;
		LoadingScreen.PlaybackType = EMoviePlaybackType::MT_LoadingLoop;
		LoadingScreen.WidgetLoadingScreen = SNullWidget::NullWidget;

		LoadingScreen.MoviePaths.Empty();
		LoadingScreen.MoviePaths.Add(TEXT("engine_startup"));
		if (GetMoviePlayer()) 
		{
			GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
			GetMoviePlayer()->OnMovieClipFinished().AddRaw(this, &FObserverMovieModule::OnMovieClipFinished);
		}
#endif
	}

	virtual bool IsGameModule() const override
	{
		return true;
	}

#if !UE_SERVER

	virtual FOnClipFinished& OnClipFinished() override { return OnClipFinishedDelegate; }

	virtual void OnMovieClipFinished(const FString& ClipName)
	{
		OnClipFinished().Broadcast(ClipName);
	}

	virtual void PlayMovie(const FString& MoviePlayList, TSharedPtr<SWidget> SlateOverlayWidget, bool bSkippable, bool bAutoComplete, TEnumAsByte<EMoviePlaybackType> PlaybackType, bool bForce) override
	{
		if ( IsMoviePlayerEnabled() && !MoviePlayList.IsEmpty() && (!GetMoviePlayer()->IsMovieCurrentlyPlaying() || bForce) )
		{
			// If we are focing a move to play, stop any existing movie.		
			if (bForce && GetMoviePlayer()->IsMovieCurrentlyPlaying())
			{
				StopMovie();
			}

			FLoadingScreenAttributes LoadingScreen;
			LoadingScreen.bAutoCompleteWhenLoadingCompletes = bAutoComplete;
			LoadingScreen.bMoviesAreSkippable = bSkippable;
			LoadingScreen.MoviePaths.Empty();

			if ( FParse::Param( FCommandLine::Get(), TEXT( "nomovie" )) )
			{
				LoadingScreen.MoviePaths.Add(TEXT("load_generic_nosound"));
			}
			else
			{
				MoviePlayList.ParseIntoArray(LoadingScreen.MoviePaths, TEXT(";"), true);
			}

			LoadingScreen.PlaybackType = PlaybackType;
			LoadingScreen.WidgetLoadingScreen = SlateOverlayWidget; 

			if (GetMoviePlayer())
			{
				GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
				GetMoviePlayer()->PlayMovie();
			}
		}
	}

	virtual void StopMovie() override
	{
		if (GetMoviePlayer()) GetMoviePlayer()->StopMovie();
	}

	virtual void WaitForMovieToFinished() 
	{
		if (GetMoviePlayer())
		{
			GetMoviePlayer()->WaitForMovieToFinish();
		}
	}

	virtual bool IsMoviePlaying() override
	{
		return GetMoviePlayer() ? GetMoviePlayer()->IsMovieCurrentlyPlaying() : false;
	}

	virtual void SetSlateOverlayWidget(TSharedPtr<SWidget> NewSlateOverlayWidget) override
	{
		if (GetMoviePlayer()) GetMoviePlayer()->SetSlateOverlayWidget(NewSlateOverlayWidget);
	}

protected:

	FOnClipFinished OnClipFinishedDelegate;

#endif
};

IMPLEMENT_GAME_MODULE(FObserverMovieModule, ObserverMovie);
