#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "OBAnimTypes.generated.h"

/**
 * Data asset to hold facial animation curve values.
 */
UCLASS(BlueprintType)
class UOBAnimTypes : public UDataAsset
{
	GENERATED_BODY()

public:

	UOBAnimTypes();

	/** Brows Facial Curves */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Observer")
	TMap<FName, float> Brows;
	/** Eyes Facial Curves */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Observer")
	TMap<FName, float> Eyes;
	/** Blinks Facial Curves */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Observer")
	TMap<FName, float> Blinks;
	/** Nose Facial Curves */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Observer")
	TMap<FName, float> Nose;
	/** Cheeks Facial Curves */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Observer")
	TMap<FName, float> Cheeks;
	/** Mouth Facial Curves */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Observer")
	TMap<FName, float> Mouth;
	/** Jaw Facial Curves */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Observer")
	TMap<FName, float> Jaw;
};
