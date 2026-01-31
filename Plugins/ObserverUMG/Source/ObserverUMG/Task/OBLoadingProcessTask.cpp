#include "OBLoadingProcessTask.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "OBUILoadingSubsystem.h"
#include "UObject/ScriptInterface.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OBLoadingProcessTask)

UOBLoadingProcessTask* UOBLoadingProcessTask::CreateLoadingScreenProcessTask(UObject* WorldContextObject, const FString& ShowLoadingScreenReason)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr;
	UOBUILoadingSubsystem* LoadingScreenManager = GameInstance ? GameInstance->GetSubsystem<UOBUILoadingSubsystem>() : nullptr;

	if (LoadingScreenManager)
	{
		UOBLoadingProcessTask* NewLoadingTask = NewObject<UOBLoadingProcessTask>(LoadingScreenManager);
		NewLoadingTask->SetShowLoadingScreenReason(ShowLoadingScreenReason);

		LoadingScreenManager->RegisterLoadingProcessor(NewLoadingTask);
		
		return NewLoadingTask;
	}

	return nullptr;
}

void UOBLoadingProcessTask::Unregister()
{
	UOBUILoadingSubsystem* LoadingScreenManager = Cast<UOBUILoadingSubsystem>(GetOuter());
	LoadingScreenManager->UnregisterLoadingProcessor(this);
}

void UOBLoadingProcessTask::SetShowLoadingScreenReason(const FString& InReason)
{
	Reason = InReason;
}

bool UOBLoadingProcessTask::ShouldShowLoadingScreen(FString& OutReason) const
{
	OutReason = Reason;
	return true;
}
