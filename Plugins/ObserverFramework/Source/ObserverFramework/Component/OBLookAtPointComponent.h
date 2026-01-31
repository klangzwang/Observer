#pragma once
#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "OBLookAtPointComponent.generated.h"

UCLASS(ClassGroup = (Observer), Blueprintable, meta = (BlueprintSpawnableComponent))
class OBSERVERFRAMEWORK_API UOBLookAtPointComponent : public USceneComponent
{
	GENERATED_BODY()

public:	

	UOBLookAtPointComponent();

	/*Returns a target point actor used for tracking*/
	UFUNCTION(BlueprintCallable, Category = Observer)
	class ATargetPoint* GetTargetPoint();

	UFUNCTION(BlueprintPure, Category = Observer)
	FORCEINLINE FName GetPointName() const
	{
		return PointName;
	}

protected:

	UPROPERTY(EditDefaultsOnly, Category = Observer)
	FName PointName;

private: 
		
	UPROPERTY()
	class ATargetPoint* targetPoint;
};
