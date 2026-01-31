#pragma once
#include "CoreMinimal.h"
#include "UObject/SoftObjectPtr.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OBUIManagerSubsystem.generated.h"

class UCommonActivatableWidget;
class FSubsystemCollectionBase;
class UOBLocalPlayer;
class UOBHUDLayout;
class UOBUIGamePolicy;
class UObject;

struct FComponentRequestHandle;

UCLASS(MinimalAPI, meta = (DisplayName="UI Manager Subsystem", Category = Observer), config = Game)
class UOBUIManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	UOBUIManagerSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	const UOBUIGamePolicy* GetCurrentUIPolicy() const { return CurrentPolicy; }
	UOBUIGamePolicy* GetCurrentUIPolicy() { return CurrentPolicy; }

	UFUNCTION(BlueprintCallable, Category = "Observer|UIManager")
	virtual void NotifyPlayerAdded(UOBLocalPlayer* LocalPlayer);

	UFUNCTION(BlueprintCallable, Category = "Observer|UIManager")
	virtual void NotifyPlayerRemoved(UOBLocalPlayer* LocalPlayer);

	UFUNCTION(BlueprintCallable, Category = "Observer|UIManager")
	virtual void NotifyPlayerDestroyed(UOBLocalPlayer* LocalPlayer);

private:

	bool Tick(float DeltaTime);
	void SyncRootLayoutVisibilityToShowHUD();

	FTSTicker::FDelegateHandle TickHandle;

protected:

	void SwitchToPolicy(UOBUIGamePolicy* InPolicy);

private:

	UPROPERTY(Transient)
	TObjectPtr<UOBUIGamePolicy> CurrentPolicy = nullptr;

	UPROPERTY(config, EditAnywhere)
	TSoftClassPtr<UOBUIGamePolicy> DefaultUIPolicyClass;

//public:

	//UFUNCTION(BlueprintCallable, Category = "Observer|UIManager")
	//void PushMenuWidget(TSoftClassPtr<UCommonActivatableWidget> WidgetClass);
};
