#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "IOBEntityInterface.h"
#include "OBTypes.h"
#include "GenericTeamAgentInterface.h"
#include "OBAICharacter.generated.h"

class UAnimInstance;
class UAnimMontage;

UCLASS(config = Game)
class AOBAICharacter : public ACharacter, public IGenericTeamAgentInterface, public IOBEntityInterface
{
	GENERATED_BODY()
	
public:

	AOBAICharacter();

	virtual float PlayAnimMontage(class UAnimMontage* AnimMontage, float InPlayRate = 1.f, FName StartSectionName = NAME_None) override;
	virtual void StopAnimMontage(class UAnimMontage* AnimMontage) override;
	void StopAllAnimMontages(float blendout);

public:

	UFUNCTION(BlueprintCallable, Category = "Observer|AICharacter")
	void AssignTeam(ETeam team);

	UFUNCTION(BlueprintPure, Category = "Observer|AICharacter")
	ETeam GetCombatTeam() const;

	UPROPERTY()
	ETeam CombatTeam = ETeam::ETeam2;

public:

	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamId) override;
	virtual FGenericTeamId GetGenericTeamId() const override;

protected:

	UPROPERTY(EditAnywhere, Category = "Observer|AICharacter")
	FGenericTeamId GenericTeamId;
};
