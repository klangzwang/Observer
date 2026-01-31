#include "OBGamePhaseComponent.h"

UOBGamePhaseComponent::UOBGamePhaseComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UOBGamePhaseComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UOBGamePhaseComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void UOBGamePhaseComponent::StartPhase(FGameplayTag IncomingPhaseTag)
{
	if (!IncomingPhaseTag.IsValid())
		return;

	for (int32 i = ActivePhases.Num() - 1; i >= 0; --i)
	{
		const FGameplayTag& ActiveTag = ActivePhases[i];
		if (!IncomingPhaseTag.MatchesTag(ActiveTag))
			EndPhase(ActiveTag);
	}

	if (!ActivePhases.Contains(IncomingPhaseTag))
	{
		ActivePhases.Add(IncomingPhaseTag);
		OnPhaseStarted.Broadcast(IncomingPhaseTag);
	}
}

void UOBGamePhaseComponent::EndPhase(FGameplayTag PhaseTag)
{
	if (!PhaseTag.IsValid())
		return;

	if (ActivePhases.Contains(PhaseTag))
	{
		ActivePhases.Remove(PhaseTag);
		OnPhaseEnded.Broadcast(PhaseTag);
	}
}

bool UOBGamePhaseComponent::IsPhaseActive(FGameplayTag PhaseTag) const
{
	for (const FGameplayTag& ActiveTag : ActivePhases)
	{
		if (ActiveTag.MatchesTag(PhaseTag)) return true;
	}
	return false;
}
