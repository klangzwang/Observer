#pragma once
#include "Components/ActorComponent.h"
#include "OBHealthComponent.generated.h"

class UOBHealthComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOBHealthChanged, UOBHealthComponent*, HealthComponent, float, OldValue, float, NewValue, AActor*, Instigator);

UCLASS(MinimalAPI, Blueprintable, Meta=(BlueprintSpawnableComponent))
class UOBHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UOBHealthComponent(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintPure, Category = "Observer|Health")
	static UOBHealthComponent* FindHealthComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UOBHealthComponent>() : nullptr); }
	
public:

	UFUNCTION(BlueprintCallable, Category = "Observer|Health")
	void InitHealth();

	UFUNCTION(BlueprintCallable, Category = "Observer|Health")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Observer|Health")
	void SetHealth(float InHealth);

	UFUNCTION(BlueprintCallable, Category = "Observer|Health")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Observer|Health")
	float GetHealthNormalized() const;

	virtual void DamageSelfDestruct(bool bFellOutOfWorld = false);

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Observer|Health")
	float CurrentHealth = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Observer|Health")
	float MaxHealth = 100.f;

public:

	UPROPERTY(BlueprintAssignable, Category = "Observer|Health")
	FOBHealthChanged OnHealthChanged;

protected:

	UFUNCTION(BlueprintCallable, Category = "Observer|Health")
	virtual void HandleHealthChanged(AActor* DamageInstigator, AActor* DamageCauser, float DamageMagnitude, float OldValue, float NewValue);
};
