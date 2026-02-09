#include "OBWorldSettings.h"
#include "ObserverFramework.h"
#include "OBGameEngine.h"
#include "OBLocalPlayer.h"
#include "EngineUtils.h"
#include "Engine/World.h"
#include "Engine/LevelScriptActor.h"
#include "Components/AudioComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OBWorldSettings)

AOBWorldSettings::AOBWorldSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

AOBWorldSettings* AOBWorldSettings::GetWorldSettings(UObject* WorldContextObject)
{
	UWorld* World = (WorldContextObject != NULL) ? WorldContextObject->GetWorld() : NULL;
	return (World != NULL) ? Cast<AOBWorldSettings>(World->GetWorldSettings()) : NULL;
}

void AOBWorldSettings::NotifyBeginPlay()
{
	UWorld* World = GetWorld();
	if (!World->GetBegunPlay())
	{
		World->SetBegunPlay(true);
		// in order to avoid double calls to newly added actors we need to track what we started with
		TArray<AActor*> FullActorList;
		TArray<AActor*> LevelActorList;
		FullActorList.Reserve(World->PersistentLevel->Actors.Num());
		for (FActorIterator It(World); It; ++It)
		{
			// prioritize Level script actors to be called at last to have all other Actors being initialized
			if (It->IsA(ALevelScriptActor::StaticClass()))
			{
				LevelActorList.Add(*It);
			}
			else
			{
				FullActorList.Add(*It);
			}
		}
		FullActorList.Append(LevelActorList);
		for (AActor* Actor : FullActorList)
		{
			Actor->DispatchBeginPlay();
		}
	}
}

void AOBWorldSettings::BeginPlay()
{
	if (Music != NULL && GetNetMode() != NM_DedicatedServer)
	{
		bool bPlayMusic = true;

		UOBLocalPlayer* LP = Cast<UOBLocalPlayer>(GEngine->GetFirstGamePlayer(GetWorld()));
		if (LP)
		{
			bPlayMusic = false;
		}

		if (bPlayMusic)
		{
			MusicComp = NewObject<UAudioComponent>(this);
			MusicComp->bAllowSpatialization = false;
			MusicComp->bShouldRemainActiveIfDropped = true;
			MusicComp->SetSound(Music);
			MusicComp->Play();
		}
	}

	Super::BeginPlay();

	if (!IsPendingKillPending())
	{
		//FTimerHandle TempHandle;
		//GetWorldTimerManager().SetTimer(TempHandle, this, &AOBWorldSettings::ExpireImpactEffects, 0.5f, true);
	}
}

void AOBWorldSettings::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorldTimerManager().ClearAllTimersForObject(this);
}