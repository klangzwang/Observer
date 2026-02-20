#pragma once
#include "GameFramework/Actor.h"
#include "GenericTeamAgentInterface.h"
#include "OBDeviceBase.generated.h"

class UCapsuleComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCompletedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUpdateProgressDelegate, int32, InCurrentProgress);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUpdateSegmentDelegate, int32, InCurrentSegment);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUpdateStatusDelegate, bool, InNewStatus);

UCLASS(MinimalAPI, Blueprintable, BlueprintType)
class AOBDeviceBase : public AActor
{
	GENERATED_BODY()

public:

	AOBDeviceBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Observer|Device")
	UCapsuleComponent* Capsule;

protected:
	
	virtual void BeginPlay() override;
	virtual void RegisterDevice(AOBDeviceBase* DeviceActor);

//
// Basics
//
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Observer|Device")
	FName DeviceName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Observer|Device")
	float CurrentProgress;

public:

	UFUNCTION(BlueprintCallable, Category = "Observer|Device")
	FName GetDeviceName() const { return DeviceName; }

//
// Collision
//
public:

	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Observer|Device")
	void OnOverlapBeginEnd(ACharacter* OtherCharacter, bool OverlapBegin);
	//void OnOverlapBeginEnd(ACharacter* OtherCharacter, const TArray<ACharacter*>& InCharactersInCapsule, bool OverlapBegin, bool InIsActive);

protected:

	UPROPERTY(BlueprintReadOnly, Category = "Observer|Device")
	TArray<ACharacter*> CharactersInCapsule;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Observer|Device")
	TArray<ACharacter*> CapsulePlayers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Observer|Device")
	TArray<ACharacter*> CapsuleEnemys;

//
// Delegate
//
public:

	UPROPERTY(BlueprintAssignable, Category = "Observer|Device")
	FOnUpdateProgressDelegate OnUpdateProgressDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Observer|Device")
	FOnUpdateSegmentDelegate OnUpdateSegmentDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Observer|Device")
	FOnUpdateStatusDelegate OnUpdateStatusDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Observer|Device")
	FOnCompletedDelegate OnCompletedDelegate;
};
