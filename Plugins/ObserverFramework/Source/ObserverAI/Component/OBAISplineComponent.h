#pragma once
#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "Components/BillboardComponent.h"
#include "OBAISplineComponent.generated.h"

class FPrimitiveSceneProxy;
class FPrimitiveDrawInterface;
class FSceneView;

USTRUCT(BlueprintType)
struct OBSERVERAI_API FAISplineData
{
	GENERATED_BODY()

public:

	FAISplineData()
	{
	}

	UPROPERTY()
	int32 SplinePointSize = 12;

	UPROPERTY()
	float SplineThickness = 2.f;

	UPROPERTY()
	FLinearColor SplineLineColor = FColor::Cyan;

	UPROPERTY()
	FLinearColor SplinePointColor = FColor::Yellow;
};

UCLASS(ClassGroup=(Custom), Blueprintable, ShowCategories = (Rendering), meta=(BlueprintSpawnableComponent))
class UOBAISplineComponent : public USplineComponent
{
	GENERATED_BODY()

#if WITH_EDITORONLY_DATA

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Observer|AISpline", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UBillboardComponent> Icon;

public:

	class UBillboardComponent* GetIcon() const;

#endif

public:

	UOBAISplineComponent();

protected:

	virtual void BeginPlay() override;
	//virtual void OnRegister() override;
	//virtual void OnUnregister() override;

//#if WITH_EDITOR
//	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
//#endif

public:

	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;

	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Observer|AISpline")
	FVector GetNextWaypointLocation(float DistanceAlongSpline);

private:

	friend class FSplineSceneProxy;

private:

	bool bIsBound;

	float ArrowMoverValue = 0.f;

public:

	UFUNCTION(BlueprintCallable, Category = "Observer|AISpline")
	void SetBoundToPath(bool IsBounded);

	UFUNCTION(BlueprintCallable, Category = "Observer|AISpline")
	bool IsBoundToPath() { return bIsBound == true; };

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Observer|AISpline")
	FAISplineData AISplineData;
};
