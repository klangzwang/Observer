#pragma once
#include "GameFramework/Actor.h"
#include "OBStealth.generated.h"

class UStaticMeshComponent;
class UBoxComponent;

UCLASS(MinimalAPI, Blueprintable, BlueprintType)
class AOBStealth : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Observer|Stealth", meta = (AllowPrivateAccess = "true"))
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Observer|Stealth", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Observer|Stealth", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* Box;

public:

	AOBStealth();

protected:

	virtual void BeginPlay() override;

private:

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Observer|Stealth", meta = (AllowPrivateAccess = true))
	USoundBase* StealthInSound;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Observer|Stealth", meta = (AllowPrivateAccess = true))
	USoundBase* StealthOutSound;
};
