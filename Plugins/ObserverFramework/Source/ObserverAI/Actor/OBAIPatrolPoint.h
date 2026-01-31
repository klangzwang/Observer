#pragma once
#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "GameFramework/Actor.h"
#include "OBAIPatrolPoint.generated.h"

UCLASS(MinimalAPI)
class AOBAIPatrolPoint : public AActor
{
	GENERATED_UCLASS_BODY()
	
#if WITH_EDITORONLY_DATA

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Observer|Patrol", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UBillboardComponent> Icon;

public:

	class UBillboardComponent* GetIcon() const;

#endif

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Observer|Patrol", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UArrowComponent> Arrow;

public:

	UFUNCTION(BlueprintCallable, Category = "Observer|Patrol")
	UArrowComponent* GetArrow();

public:

	virtual void OnConstruction(const FTransform& Transform) override;
};
