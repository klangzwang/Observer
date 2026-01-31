#pragma once
#include "EnhancedInputComponent.h"
#include "EnhancedActionKeyMapping.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "Components/PawnComponent.h"
#include "Engine/DataTable.h"
#include "Components/Image.h"
#include "Sound/SoundBase.h"
//#include "OBWeapon.h"
#include "OBDelegate.h"
#include "OBCharacter.h"
#include "OBHeroComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInputUsingGateDelegate, FInputUsingGate, InUsingGate);

namespace EEndPlayReason { enum Type : int; }
struct FLoadedMappableConfigPair;
struct FMappableConfigPair;

class UGameFrameworkComponentManager;
class UInputComponent;
class UInputMappingContext;
class UOBInputConfig;
class UObject;
class UAnimMontage;
class UCanvas;
struct FActorInitStateChangedParams;
struct FFrame;
struct FGameplayTag;
struct FInputActionValue;

UENUM(Meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum EInputFlags
{
	INP_None = 0x0 UMETA(DisplayName = "None"),
	INP_Forward = 0x1 UMETA(DisplayName = "Forward"),
	INP_Backward = 0x2 UMETA(DisplayName = "Backward"),
	INP_Left = 0x4 UMETA(DisplayName = "Left"),
	INP_Right = 0x8 UMETA(DisplayName = "Right"),
	INP_ForwardLeft = INP_Forward + INP_Left UMETA(Hidden),
	INP_ForwardRight = INP_Forward + INP_Right UMETA(Hidden),
	INP_BackwardLeft = INP_Backward + INP_Left UMETA(Hidden),
	INP_BackwardRight = INP_Backward + INP_Right UMETA(Hidden),
	INP_Neutral = 0x10 UMETA(DisplayName = "Neutral"),
	INP_A = 0x20 UMETA(DisplayName = "A"),
	INP_B = 0x40 UMETA(DisplayName = "B"),
	INP_C = 0x80 UMETA(DisplayName = "C"),
	INP_D = 0x100 UMETA(DisplayName = "D"),
	INP_E = 0x200 UMETA(DisplayName = "E"),
	INP_F = 0x400 UMETA(DisplayName = "F"),
	INP_G = 0x800 UMETA(DisplayName = "G"),
	INP_H = 0x1000 UMETA(DisplayName = "H"),
};

ENUM_CLASS_FLAGS(EInputFlags);

UENUM(Meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum EEnableFlags
{
	ENB_None = 0x0 UMETA(Hidden),
	ENB_Standing = 0x1 UMETA(DisplayName = "Standing"),
	ENB_Crouching = 0x2 UMETA(DisplayName = "Crouching"),
	ENB_Rolling = 0x80 UMETA(DisplayName = "Rolling"),
	ENB_Sliding = 0x100 UMETA(DisplayName = "Sliding"),
	ENB_Sprinting = 0x40 UMETA(DisplayName = "Sprinting"),
	ENB_ForwardWalk = 0x4 UMETA(DisplayName = "Forward Walk"),
	ENB_BackWalk = 0x8 UMETA(DisplayName = "Backward Walk"),
	ENB_LeftStrafe = 0x10 UMETA(DisplayName = "Left Strafe"),
	ENB_RightStrafe = 0x20 UMETA(DisplayName = "Right Strafe")
};

ENUM_CLASS_FLAGS(EEnableFlags);

USTRUCT(BlueprintType)
struct FActionInstance
{
	GENERATED_BODY()

private:

	UPROPERTY(BlueprintReadOnly, Category = "Input", Meta = (AllowPrivateAccess))
	FInputActionInstance InputActionInstance;

	UPROPERTY(BlueprintReadOnly, Category = "Input", Meta = (AllowPrivateAccess))
	TObjectPtr<const AOBCharacter> SourceCharacter = nullptr;

public:

	FActionInstance() = default;
	FActionInstance(const FInputActionInstance InInputActionInstance);

	FInputActionInstance GetInputActionInstance() const { return InputActionInstance; }

	ETriggerEvent GetTriggerEvent() const { return InputActionInstance.GetTriggerEvent(); }

	TObjectPtr<const UInputAction> GetSourceAction() const { return InputActionInstance.GetSourceAction(); }

	TObjectPtr<const AOBCharacter> GetCharacter() const { return SourceCharacter; }
};

USTRUCT(BlueprintType)
struct FCharacterClass : public FTableRowBase
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
	FText mCharacterName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
	UTexture2D* mCharacterIcon = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
	USkeletalMesh* BaseMesh = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
	UAnimMontage* ExampleMontage = nullptr;

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
	UAnimMontage* SlideForward = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
	UAnimMontage* RollForward = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
	UAnimMontage* RollBackward = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
	UAnimMontage* RollLeft = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
	UAnimMontage* RollRight = nullptr;

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
	bool bIsCrouchingAtStart = false;

//	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
//	TSubclassOf<AOBWeapon> mWeapon;

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<USoundBase> SpawnSound;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<USoundBase> HitSound;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<USoundBase> StaminaStartBreathingAudio;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<USoundBase> StaminaStopBreathingAudio;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<USoundBase> HealthStartHeartbeatAudio;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<USoundBase> HealthStopHeartbeatAudio;
};

UCLASS(MinimalAPI, Blueprintable, Meta=(BlueprintSpawnableComponent))
class UOBHeroComponent : public UPawnComponent
{
	GENERATED_BODY()

public:

	UOBHeroComponent(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintPure, Category = "Observer|Hero")
	static UOBHeroComponent* FindHeroComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UOBHeroComponent>() : nullptr); }

	void AddAdditionalInputConfig(const UOBInputConfig* mInputConfig);

	bool IsReadyToBindInputs() const;

protected:

	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void DispDebug(UCanvas* Canvas, float YL, float YPos);

	virtual void InitializeComponent() override;

	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Observer|Movement")
	float MaxWalkSpeedDefault;

public:

	virtual void InitializePlayerInput(UInputComponent* PlayerInputComponent);

protected:

	bool bReadyToBindInputs;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Observer|Input", meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<const UInputMappingContext>> InputMappingContexts;

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Observer|Input")
	TObjectPtr<UOBInputConfig> InputConfig;

public:

	virtual void MovementRouting(const FInputActionInstance& Instance);

private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Observer|Hero", meta = (AllowPrivateAccess = "true"))
	FCharacterClass CharacterClass;

public:

	UFUNCTION(BlueprintCallable, Category = "Observer|Hero")
	FCharacterClass GetCharacterClass() { return CharacterClass; };

public:

	UFUNCTION(BlueprintCallable, Category = "Observer|Movement")
	FORCEINLINE AOBCharacter* GetCharacter() const { return Cast<AOBCharacter>(GetOwner()); }

	UFUNCTION(BlueprintCallable, Category = "Observer|Movement")
	FORCEINLINE UOBAnimInstance* GetAnimInstance() const { return Cast<UOBAnimInstance>(GetCharacter()->GetMesh()->GetAnimInstance()); }

//
// INPUTGATE
//
public:

	UFUNCTION(BlueprintCallable, Category = "Observer|Movement")
	void SetInputUsingGate(FInputUsingGate InInputUsingGate);

	UFUNCTION(BlueprintCallable, Category = "Observer|Movement")
	FORCEINLINE FInputUsingGate GetUsingInputGate() const { return mInputUsingGate; }

	UPROPERTY(BlueprintAssignable)
	FInputUsingGateDelegate mInputUsingGateDelegate;

private:

	FInputUsingGate mInputUsingGate;

protected:

	FOnMontageEnded OnCrouchMontageEnded;

	void CrouchingMontageEnded(UAnimMontage* Montage, bool bInterrupted);

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Bitmask, BitmaskEnum = "/Script/ObserverGame.EInputFlags"))
	int Flag;
	
protected:

	UPROPERTY(BlueprintReadOnly, Category = "Observer|Outlines")
	class UPostProcessComponent* postProcessComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Observer|Outlines")
	class UMaterialInterface* outlineMaterial = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Observer|Outlines")
	class UTexture2D* nullTexture = nullptr;

public:

	UPROPERTY(BlueprintReadWrite, Category = "Observer|Outlines")
	class UMaterialInstanceDynamic* dynamicMaterial = nullptr;

public:

	int32 MovingNoiseFrames;
};
