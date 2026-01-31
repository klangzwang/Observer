#pragma once
#include "CoreMinimal.h"
#include "Components/GameStateComponent.h"
#include "GameplayTagContainer.h"
#include "OBGamePhaseComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPhaseTagEvent, const FGameplayTag&, PhaseTag);

UCLASS(meta=(BlueprintSpawnableComponent))
class UOBGamePhaseComponent : public UGameStateComponent
{
	GENERATED_BODY()

public:	

	UOBGamePhaseComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UFUNCTION(BlueprintCallable, Category = "Observer|GamePhase")
	void StartPhase(UPARAM(meta = (Categories = "GamePhase")) FGameplayTag PhaseTag);

	UFUNCTION(BlueprintCallable, Category = "Observer|GamePhase")
	void EndPhase(UPARAM(meta = (Categories = "GamePhase")) FGameplayTag PhaseTag);

	UFUNCTION(BlueprintCallable, Category = "Observer|GamePhase")
	bool IsPhaseActive(UPARAM(meta = (Categories = "GamePhase")) FGameplayTag PhaseTag) const;

	UPROPERTY(BlueprintAssignable, Category = "Observer|GamePhase")
	FPhaseTagEvent OnPhaseStarted;

	UPROPERTY(BlueprintAssignable, Category = "Observer|GamePhase")
	FPhaseTagEvent OnPhaseEnded;

protected:

	UPROPERTY()
	TArray<FGameplayTag> ActivePhases;
};