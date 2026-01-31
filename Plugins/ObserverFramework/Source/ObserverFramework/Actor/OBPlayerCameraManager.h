#pragma once
#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "OBCharacter.h"
#include "OBPlayerCameraManager.generated.h"

/**
 * Delegate that is called when a camera sequence starts.
 * @param cameraSequence - The name of the camera sequence that started.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCameraSequenceStarted, const FName&, cameraSequence);

/**
 * Delegate that is called when a camera sequence ends.
 * @param cameraSequence - The name of the camera sequence that ended.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCameraSequenceEnded, const FName&, cameraSequence);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBlackFadeStarted, bool, bIsFadeToBlack);

//
//
//
UENUM(BlueprintType)
enum class ELockType : uint8
{
    EYawOnly = 0 UMETA(DisplayName = "Only Lock Yaw"),
    EAllAxis UMETA(DisplayName = "Lock Yaw And Pitch"),
};

//
//
//
UENUM(BlueprintType)
enum class ETargetLockType : uint8
{
    ENone = 0,
    EActor,
    EComponent,
};

//
//
//
USTRUCT(BlueprintType)
struct FCameraMovementSettings : public FTableRowBase
{
    GENERATED_BODY()

public:

    FCameraMovementSettings()
    {
        CameraOffset = FVector();
        Shake = UCameraShakeBase::StaticClass();
        FOV = 0.f;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Offset")
    FVector CameraOffset;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Offset")
    float InterpSpeed = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float FovInterpSpeed = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float FOV;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    TSubclassOf<UCameraShakeBase> Shake;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bIsWorldShaking"), Category = "Camera")
    float ShakeIntensity = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    bool bShakeLooping = false;

    FORCEINLINE FCameraMovementSettings operator+=(const FCameraMovementSettings& Other)
    {

        this->CameraOffset += Other.CameraOffset;
        this->FOV += Other.FOV;
        this->InterpSpeed = Other.InterpSpeed;
        this->FovInterpSpeed = Other.FovInterpSpeed;

        if (Other.Shake) {
            this->Shake = Other.Shake;
            this->ShakeIntensity = Other.ShakeIntensity;
            this->bShakeLooping = Other.bShakeLooping;
        }

        return *this;
    }

    FORCEINLINE FCameraMovementSettings operator-=(const FCameraMovementSettings& Other)
    {

        this->CameraOffset = this->CameraOffset - Other.CameraOffset;
        this->FOV -= Other.FOV;
        this->InterpSpeed = Other.InterpSpeed;
        this->FovInterpSpeed = Other.FovInterpSpeed;
        if (Other.Shake) {
            this->Shake = nullptr;
            this->bShakeLooping = false;
        }

        return *this;
    }
};

//
//
//
USTRUCT(BlueprintType)
struct FCurrentSequence {

    GENERATED_BODY()

    FCurrentSequence()
    {
        cameraSequenceComp = nullptr;
    }

public:

    UPROPERTY(BlueprintReadOnly, Category = "Camera")
    class UOBCameraSplineComponent* cameraSequenceComp;

    UPROPERTY(BlueprintReadOnly, Category = "Camera")
    float currentTime = 0.f;

    UPROPERTY(BlueprintReadOnly, Category = "Camera")
    float currentSplinePos = 0.f;

    void Reset(UOBCameraSplineComponent* inSequence)
    {
        cameraSequenceComp = inSequence;
        currentTime = 0.f;
        currentSplinePos = 0.f;
    }
};

//
//
//
USTRUCT(BlueprintType)
struct FSequenceEvent {

    GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    bool bEditCameraSpeed = false;

    UPROPERTY(EditAnywhere, meta = (EditCondition = bEditCameraSpeed), Category = "Camera")
    float CameraSpeed = 0.3f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    bool bEditFov = false;

    UPROPERTY(EditAnywhere, meta = (EditCondition = bEditFov), Category = "Camera")
    float CameraFovOffset = 0.f;

    UPROPERTY(EditAnywhere, meta = (EditCondition = bEditFov), BlueprintReadWrite, Category = "Camera")
    float FovOffsetInterpSpeed = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    bool bSwitchLookAt = false;

    UPROPERTY(EditAnywhere, meta = (EditCondition = bSwitchLookAt), BlueprintReadWrite, Category = "Camera")
    FName LookAtPoint;

    UPROPERTY(EditAnywhere, meta = (EditCondition = bSwitchLookAt), BlueprintReadWrite, Category = "Camera")
    float LookAtRotationSpeed = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    bool bChangeTimeDilation = false;

    UPROPERTY(EditAnywhere, meta = (EditCondition = bChangeTimeDilation), BlueprintReadWrite, Category = "Camera")
    float TimeDilatation = 1.0f;
};

//
//
//
USTRUCT(BlueprintType)
struct FCameraSequenceSettings : public FTableRowBase {

    GENERATED_BODY()

public:

    FCameraSequenceSettings()
    {
        lookAtActor = nullptr;
    }

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera")
    float CameraSpeed = 0.5f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera")
    float CameraRotationsSpeed = 1.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera")
    float CameraFov = 90.f;

    UPROPERTY(BlueprintReadOnly, Category = "Camera")
    float FovInterpSpeed = 1.f;

    UPROPERTY(BlueprintReadOnly, Category = "Camera")
    AActor* lookAtActor;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera")
    FName LookAtPoint;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera")
    float TimeDilatation = 1.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera")
    float BlendSettingsTime = .3f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera")
    float OutBlendSettings = 1.f;
};

//
//
//
UCLASS()
class AOBPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

public: 

    AOBPlayerCameraManager();

protected:

    virtual void BeginPlay() override;

public:

    TObjectPtr<const AOBCharacter> GetCharacter() const { return Cast<AOBCharacter>(PCOwner->GetPawn()); }

protected:

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Observer|Camera")
    class USpringArmComponent* SpringArm;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Observer|Camera")
    class UCameraComponent* PlayerCamera;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Observer|Camera")
    FCameraMovementSettings FinalMov;

    /** Class type for the camera actor to use */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Observer|Camera")
    TSubclassOf<ACameraActor> CameraActorClass;

    UPROPERTY()
    FVector originalPos;

    UPROPERTY()
    float originalFov;

    void UpdateCamera(float deltaTime);

private:

    void MoveCamera(float deltaTime);

    void FollowPlayer(float deltaTime);

public:

    UFUNCTION(BlueprintCallable, Category = "Observer|Camera")
    void SetCamera(FVector Location, float FOV = 0.f, float Speed = 1.f, float FovSpeed = 1.f);

    UPROPERTY(EditDefaultsOnly, Category = "Observer|Camera")
    bool bFollowPlayer = false;

    UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "bFollowPlayerWhenMoving"), Category = "Observer|Camera")
    float FollowSpeed = 1.f;

    UFUNCTION(BlueprintCallable, Category = "Observer|Camera")
    void SetFollowPlayer(bool bFollow, float inFollowSpeed = 1.f)
    {
        bFollowPlayer = bFollow;
        FollowSpeed = inFollowSpeed;
    }

    UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "bFollowPlayerWhenMoving"), Category = "Observer|Camera")
    float MaxFollowDeltaAngle = 180.f;

//
//
// LOCKTO
public:

    UFUNCTION(BlueprintCallable, Category = "CCM")
    void LockCameraOnActor(class AActor* ActorLookAt, ELockType lockType, float lockStrength = 5.f);

    UFUNCTION(BlueprintCallable, Category = "CCM")
    void LockCameraOnComponent(class USceneComponent* ComponentToLookAt, ELockType lockType, float lockStrength = 5.f);

    UFUNCTION(BlueprintCallable, Category = "CCM")
    void StopLookingActor();

private:

    void UpdateLockOnTarget(float deltaTime);

    void LockCameraOnPosition(FVector localPos, float deltaTime);

protected:

    /** Offset for global camera lock */
    UPROPERTY(EditAnywhere, Category = "Camera")
    FVector2D GlobalLockOffset = FVector2D();

    /** Upper pitch limit for camera lock (in degrees) */
    UPROPERTY(EditAnywhere, Category = "Camera")
    float CameraLockPitchUpperLimit = 75.f;

    /** Lower pitch limit for camera lock (in degrees) */
    UPROPERTY(EditAnywhere, Category = "Camera")
    float CameraLockPitchLowerLimit = 35.f;

private:

    UPROPERTY()
    ETargetLockType targetLockType = ETargetLockType::ENone;

    UPROPERTY()
    float _lockStrength;

    UPROPERTY()
    ELockType currentLockType;

    UPROPERTY()
    class AActor* localtarget;

    UPROPERTY()
    class USceneComponent* localtargetComponent;

//
//
// SEQUENCE
protected:

    /** Flag indicating if a camera sequence is currently playing */
    UPROPERTY()
    bool bIsPlayingSequence = false;

    /** The current camera actor playing a sequence */
    UPROPERTY()
    TObjectPtr<class ACameraActor> sequenceCameraActor;

    /** Current sequence settings */
    UPROPERTY()
    FCurrentSequence currentSequence;

public:

    /** Event that gets triggered when a camera sequence starts */
    UPROPERTY(BlueprintAssignable, Category = "Camera")
    FOnCameraSequenceStarted OnCameraSequenceStarted;

    /** Event that gets triggered when a camera sequence ends */
    UPROPERTY(BlueprintAssignable, Category = "Camera")
    FOnCameraSequenceEnded OnCameraSequenceEnded;

    /** Trigger a camera sequence by event name and specify the camera actor owner */
    UFUNCTION(BlueprintCallable, Category = "Camera")
    void TriggerCameraSequence(AActor* cameraOwner, FName CameraEventName);

    /** Stop the current camera sequence */
    UFUNCTION(BlueprintCallable, Category = "Camera")
    void StopCurrentCameraSequence();

private:

    void SetCameraSequenceData();

    void UpdateCameraSequence(float deltaTime);

    UPROPERTY()
    FCameraSequenceSettings currentSequenceSettings;

//
//
// EVENT
public:

    /** Trigger a camera event by name (e.g., cinematic sequences, cutscenes) */
    UFUNCTION(BlueprintCallable, Category = "Camera")
    void TriggerCameraEvent(FName CameraEventName);

    /** Trigger a timed camera event that lasts for a specified duration */
    UFUNCTION(BlueprintCallable, Category = "Camera")
    void TriggerTimedCameraEvent(FName CameraEventName, float duration);

    /** Stop a camera event by its name */
    UFUNCTION(BlueprintCallable, Category = "Camera")
    void StopCameraEvent(FName CameraEventName);

//
// 
// FADE
private:

    UFUNCTION()
    void StartFading(bool InFadeToBlack);

public:

    UPROPERTY(BlueprintAssignable, Category = "Observer|Fade")
    FOnBlackFadeStarted OnBlackFadeDelegate;
};
