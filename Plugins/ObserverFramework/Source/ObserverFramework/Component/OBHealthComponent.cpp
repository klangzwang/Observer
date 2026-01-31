#include "OBHealthComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OBHealthComponent)

UOBHealthComponent::UOBHealthComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;
}

void UOBHealthComponent::HandleHealthChanged(AActor* DamageInstigator, AActor* DamageCauser, float DamageMagnitude, float OldValue, float NewValue)
{
	OnHealthChanged.Broadcast(this, OldValue, NewValue, DamageInstigator);
}

void UOBHealthComponent::InitHealth()
{
	SetHealth(MaxHealth);
}

float UOBHealthComponent::GetHealth() const
{
	return CurrentHealth;
}

float UOBHealthComponent::GetMaxHealth() const
{
	return MaxHealth;
}

void UOBHealthComponent::SetHealth(float InHealth)
{
	CurrentHealth = InHealth;
	HandleHealthChanged(nullptr, nullptr, 0, GetHealth(), GetHealth());
}

float UOBHealthComponent::GetHealthNormalized() const
{
	const float mHealth = GetHealth();
	const float mMaxHealth = GetMaxHealth();

	return ((MaxHealth > 0.0f) ? (mHealth / mMaxHealth) : 0.0f);
}

void UOBHealthComponent::DamageSelfDestruct(bool bFellOutOfWorld)
{
	const float DamageAmount = GetMaxHealth();
	SetHealth(DamageAmount);
}
