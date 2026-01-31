#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "OBDelegate.generated.h"

USTRUCT(BlueprintType)
struct OBSERVERFRAMEWORK_API FInputUsingGate
{
	GENERATED_USTRUCT_BODY()

public:

	FInputUsingGate() : FInputUsingGate(false)
	{
	}

	explicit FInputUsingGate(const bool disabled) :
		IsVaulting(disabled),
		IsMoving(disabled),
		IsLooking(disabled),
		IsCrouching(disabled),
		IsSprinting(disabled),
		IsSliding(disabled),
		IsDodging(disabled),
		IsAiming(disabled),
		IsAttacking(disabled),
		IsThrowing(disabled),
		IsUsing(disabled),
		IsRolling(disabled),
		IsBlocking(disabled),
		IsHiding(disabled),
		IsStealth(disabled)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Input")
	uint8 IsVaulting : 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Input")
	uint8 IsMoving : 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Input")
	uint8 IsLooking : 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Input")
	uint8 IsCrouching : 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Input")
	uint8 IsSprinting : 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Input")
	uint8 IsSliding : 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Input")
	uint8 IsDodging : 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Input")
	uint8 IsAiming : 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Input")
	uint8 IsAttacking : 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Input")
	uint8 IsThrowing : 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Input")
	uint8 IsUsing : 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Input")
	uint8 IsRolling : 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Input")
	uint8 IsBlocking : 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Input")
	uint8 IsHiding : 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Input")
	uint8 IsStealth : 1;
};
