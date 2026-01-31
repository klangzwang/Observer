#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IOBDamageable.generated.h"

UINTERFACE(MinimalAPI, NotBlueprintable)
class UOBDamageable : public UInterface
{
	GENERATED_BODY()
};

class IOBDamageable
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category="Observer|Damage")
	virtual void ApplyDamage(float ActualDamage, AActor* DamageCauser, const FVector& DamageLocation, const FVector& DamageImpulse) = 0;

	UFUNCTION(BlueprintCallable, Category = "Observer|Damage")
	virtual void AttachProjectile(AActor* DamageCauser, const FVector& DamageLocation, const FVector& DamageImpulse) = 0;
};
