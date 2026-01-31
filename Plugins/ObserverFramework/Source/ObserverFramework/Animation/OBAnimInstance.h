#pragma once
#include "Animation/AnimInstance.h"
#include "EnhancedInputComponent.h"
#include "OBDelegate.h"
#include "Engine/TimerHandle.h"
#include "OBAnimInstance.generated.h"

class UOBCharacterMovementComponent;

UENUM(BlueprintType)
enum class ETickerBlendFunction : uint8
{
    Linear,
    Curve,
    EaseIn,
    EaseOut,
    EaseInOut,

    COUNT	UMETA(Hidden)
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTickerEnded, UOBAnimInstance*, Ticker);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTickerUpdated, float, CurrentTicksValue);

UCLASS(Config = Game)
class UOBAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	UOBAnimInstance();

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTimeX) override;

protected:

	UPROPERTY(BlueprintReadOnly, Category = "Character State Data")
	float GroundDistance = -1.0f;

public:

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnUpdateUsings", ScriptName = "OnUpdateUsings"))
	void K2_OnUpdateUsings(FInputUsingGate Input);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnUpdateInputs", ScriptName = "OnUpdateInputs"))
	void K2_OnUpdateInputs(float DeltaTime);

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Head")
    float MaxHeadInterval = 5.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Head")
    float MinHeadInterval = 2.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Head")
    float HeadDuration = 0.2f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Head")
    float HeadAlpha;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Head")
    float HeadRate;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Head")
    float HeadMorphAlpha;

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
    USkeletalMeshComponent* SkeletalMeshComp;

private:

    FTimerHandle HeadTimerHandle;

    UFUNCTION()
    void StartHead();

    void AnimateHead(float DeltaTime);

    bool bIsHeadActive;

private:

    // Timer Handle zur Verwaltung des Timers
    FTimerHandle MovementTimerHandle;

    // Aktueller Wert der bewegten Zahl
    UPROPERTY(VisibleAnywhere, Category = "Random Movement")
    float CurrentValue = 0.0f;

    // Zielwert, der erreicht werden muss
    float TargetValue = 0.0f;

    // Geschwindigkeit der Bewegung (Einheiten pro Sekunde)
    UPROPERTY(EditAnywhere, Category = "Random Movement")
    float MovementSpeed = 10.0f;

    // Min/Max-Bereich für die zufällige Bewegung
    UPROPERTY(EditAnywhere, Category = "Random Movement")
    float MinRange = 10.0f;

    UPROPERTY(EditAnywhere, Category = "Random Movement")
    float MaxRange = 100.0f;

    // Intervall, in dem die 'MoveValueToTarget'-Funktion aufgerufen wird
    const float TimerRate = 0.016f; // Ca. 60 FPS

    // Startet den Bewegungszyklus neu
    void StartNewMovementCycle();

    // Wird vom Timer in kurzen Abständen aufgerufen, um den Wert zu bewegen
    void MoveValueToTarget();

protected:

	virtual void BeginOnward();
	virtual void BeginBackward();

public:

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Observer|Tick")
    ETickerBlendFunction TickBlendFunction;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Observer|Tick")
	bool PingPong;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Observer|Tick", meta = (EditCondition = "TickBlendFunction != ETickerBlendFunction::Linear"))
	float tickExponent = 1.f;

	UPROPERTY(BlueprintReadOnly, Category = "Observer|Tick")
	float ticks = 0.f;

public:

	UFUNCTION(BlueprintCallable, Category = "Observer|Tick")
	bool isEmpty() { return ticks <= 0; }

	UFUNCTION(BlueprintCallable, Category = "Observer|Tick")
	bool ended() { return ticks >= Alpha; }

	UFUNCTION(BlueprintCallable, Category = "Observer|Tick")
	void add(float tks) { ticks = FMath::Min(FMath::Max(0, tks + tks), Alpha); }

	UFUNCTION(BlueprintCallable, Category = "Observer|Tick")
	virtual void onward();

	UFUNCTION(BlueprintCallable, Category = "Observer|Tick")
	virtual void backward();

	UFUNCTION(BlueprintCallable, Category = "Observer|Tick")
	void back()
	{
		if (ticks > 0)
		{
			ticks--;
		}
	}

	UFUNCTION(BlueprintCallable, Category = "Observer|Tick")
	void backWith(float value)
	{
		if (ticks > 0)
		{
			ticks -= FMath::Min(ticks, value);
		}
	}

	UFUNCTION(BlueprintCallable, Category = "Observer|Tick")
	void reset() { ticks = 0; }

	UFUNCTION(BlueprintCallable, Category = "Observer|Tick")
	float getMax() { return Alpha; }

	UFUNCTION(BlueprintCallable, Category = "Observer|Tick")
	void setMax(float max) { Alpha = max; }

	UFUNCTION(BlueprintCallable, Category = "Observer|Tick")
	float getTicks() { return ticks; }

	UFUNCTION(BlueprintCallable, Category = "Observer|Tick")
	void setTicks(float tks) { ticks = tks; }

	UFUNCTION(BlueprintCallable, Category = "Observer|Tick")
	void setAll(float tks)
	{
		Alpha = tks;
		ticks = tks;
	}
	UFUNCTION(BlueprintCallable, Category = "Observer|Tick")
	float getEmpty() { return Alpha - ticks; }

	UFUNCTION(BlueprintCallable, Category = "Observer|Tick")
	float perCent() { return Alpha > 0 ? ticks * 100.0F / Alpha : 0; }

	UFUNCTION(BlueprintCallable, Category = "Observer|Tick")
	float subSized() { return Alpha > 0 ? ticks / Alpha : 0; }

protected:

	FTimerHandle TickForwardHandle;
	FTimerHandle TickBackwardHandle;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Observer|Tick", meta = (EditCondition = "TickBlendFunction == ETickBlendFunction::Curve"))
	FRuntimeFloatCurve TickCurve;

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Observer|Tick")
	float DurationInSeconds = 1.f;

	UPROPERTY(BlueprintReadOnly, Category = "Observer|Tick|Internal")
	int32 TotalTicksCount = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Observer|Tick")
	float Alpha = 0.f;

public:

    UFUNCTION()
    virtual void UpdateAnimations(float DeltaTimeX);

    UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "UpdateAnimation", ScriptName = "UpdateAnimation"))
    void K2_UpdateAnimation(float RandomTargetTick = 0.f, float Channel02 = 0.f, float Channel03 = 0.f, float Channel04 = 0.f);

public:

    UPROPERTY(BlueprintAssignable, Category = "Observer|Tick|Events")
    FOnTickerEnded OnFinishedForward;

    UPROPERTY(BlueprintAssignable, Category = "Observer|Tick|Events")
    FOnTickerEnded OnFinishedBackward;

    UPROPERTY(BlueprintAssignable, Category = "Observer|Tick|Events")
    FOnTickerUpdated OnTickerUpdate;

public:

    UFUNCTION(BlueprintCallable, Category = "Observer|Tick")
    void Pause();

    UFUNCTION(BlueprintCallable, Category = "Observer|Tick")
    void Resume();

    UFUNCTION(BlueprintCallable, Category = "Observer|Tick")
    void Stop();
};
