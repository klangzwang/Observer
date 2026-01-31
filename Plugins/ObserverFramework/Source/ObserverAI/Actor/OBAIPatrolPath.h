#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OBAISplineComponent.h"
#include "OBAIPatrolPath.generated.h"

UCLASS()
class AOBAIPatrolPath : public AActor
{
	GENERATED_BODY()

public:	

	AOBAIPatrolPath();

	UFUNCTION(BlueprintPure, Category = "ObserverAI|PatrolPath")
	int32 GetSplinePointNum() const;

	UFUNCTION(BlueprintPure, Category = "ObserverAI|PatrolPath")
	UOBAISplineComponent* GetSplineComponent() { return SplineComponent; };

	UFUNCTION(BlueprintCallable, Category = "ObserverAI|PatrolPath")
	bool HasBoundTo() { return SplineComponent->IsBoundToPath(); };

protected:

	virtual void BeginPlay() override;

private:

	UPROPERTY(VisibleAnywhere, Category = "ObserverAI|Components")
	UOBAISplineComponent* SplineComponent;

public:

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnGeneratePathPoints", ScriptName = "OnGeneratePathPoints"))
	void K2_OnGeneratePathPoints(FVector PointLocation);
};