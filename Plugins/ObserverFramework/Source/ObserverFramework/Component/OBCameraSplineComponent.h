#pragma once
#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "OBPlayerCameraManager.h"
#include "OBCameraSplineComponent.generated.h"

UCLASS(ClassGroup = (Observer), Blueprintable, meta = (BlueprintSpawnableComponent))
class OBSERVERFRAMEWORK_API UOBCameraSplineComponent : public USplineComponent
{
	GENERATED_BODY()

public: 

	UFUNCTION(BlueprintPure, Category = Observer)
	FORCEINLINE FName GetCameraSequenceName() const
	{
		return SequenceName;
	}

	UFUNCTION(BlueprintPure, Category = Observer)
	FORCEINLINE FCameraSequenceSettings GetCameraSequenceSettings() const
	{
		return SequenceSettings;
	}

	UFUNCTION(BlueprintPure, Category = Observer)
	class AActor* GetSequenceLookAtPointLocation() const;

	UFUNCTION(BlueprintCallable, Category = Observer)
	class AActor* GetLookAtPointLocationByName(const FName& lookAtPointName) const;

protected:

	UPROPERTY(EditDefaultsOnly, Category = Observer)
	FName SequenceName;

	UPROPERTY(EditDefaultsOnly, Category = Observer)
	FCameraSequenceSettings SequenceSettings;
};
