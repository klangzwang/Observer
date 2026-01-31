#include "OBPlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "CineCameraComponent.h"
#include "OBCameraSplineComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OBPlayerCameraManager)

AOBPlayerCameraManager::AOBPlayerCameraManager()
{
    PrimaryActorTick.bStartWithTickEnabled = true;
    CameraActorClass = ACameraActor::StaticClass();

    OnBlackFadeDelegate.AddDynamic(this, &AOBPlayerCameraManager::StartFading);
}

void AOBPlayerCameraManager::BeginPlay()
{
    Super::BeginPlay();
	
    if (!UKismetSystemLibrary::IsDedicatedServer(this))
    {
        if (GetCharacter())
        {
            PlayerCamera = GetCharacter()->FindComponentByClass<UCameraComponent>();
            SpringArm = GetCharacter()->FindComponentByClass<USpringArmComponent>();

            if (PlayerCamera && SpringArm)
            {
                originalFov = PlayerCamera->FieldOfView;
                originalPos = SpringArm->SocketOffset;

                return;
            }
        }
    }
}

void AOBPlayerCameraManager::UpdateCamera(float deltaTime)
{
    Super::UpdateCamera(deltaTime);

    if (!UKismetSystemLibrary::IsDedicatedServer(this))
    {
        if (PlayerCamera && SpringArm)
        {
            //GEngine->AddOnScreenDebugMessage(654, 0.1f, FColor::Yellow, ("Character Camera: $s", PlayerCamera->GetName()));
            //GEngine->AddOnScreenDebugMessage(653, 0.1f, FColor::White, ("Character SpringArm: $s", SpringArm->GetName()));

            MoveCamera(deltaTime);
            UpdateLockOnTarget(deltaTime);

            if (bIsPlayingSequence)
                UpdateCameraSequence(deltaTime);
        }

        FollowPlayer(deltaTime);
    }
}

void AOBPlayerCameraManager::MoveCamera(float deltaTime)
{
    if (SpringArm)
    {
        FVector targetpos = FinalMov.CameraOffset + originalPos;
        SpringArm->SocketOffset = FMath::VInterpTo(SpringArm->SocketOffset, targetpos, deltaTime, FinalMov.InterpSpeed);
    }

    if (PlayerCamera)
    {
        float finalfov = FinalMov.FOV + originalFov;
        PlayerCamera->FieldOfView = FMath::FInterpTo(PlayerCamera->FieldOfView, finalfov, deltaTime, FinalMov.FovInterpSpeed);
    }

    if (FinalMov.bShakeLooping && PCOwner)
    {
        PCOwner->ClientStartCameraShake(FinalMov.Shake, FinalMov.ShakeIntensity);
    }
}

void AOBPlayerCameraManager::FollowPlayer(float deltaTime)
{
    if (bFollowPlayer && GetCharacter() && GetCharacter()->GetController())
    {
        const FRotator OldRotation = GetCharacter()->GetController()->GetControlRotation();
        const FRotator TargetRotation = GetCharacter()->GetActorForwardVector().Rotation();
        const FRotator NewRotation = FMath::RInterpTo(OldRotation, TargetRotation, deltaTime, FollowSpeed);

        const float deltaAngle = NewRotation.Yaw - OldRotation.Yaw;

        //GEngine->AddOnScreenDebugMessage(1, 0.2f, FColor::Yellow, FString::Printf(TEXT("Rot Dif: %f"), deltaAngle));

        if (deltaAngle < MaxFollowDeltaAngle)
        {
            GetCharacter()->GetController()->SetControlRotation(FRotator(OldRotation.Pitch, NewRotation.Yaw, OldRotation.Roll));
        }
    }
}

void AOBPlayerCameraManager::SetCamera(FVector Location, float FOV, float Speed, float FovSpeed)
{
    FVector Offset = FinalMov.CameraOffset;
    FinalMov.CameraOffset = Offset + Location;
    FinalMov.InterpSpeed = Speed;

    float Fov = FinalMov.FOV;
    FinalMov.FOV = Fov + FOV;
    FinalMov.FovInterpSpeed = FovSpeed;
}

//
//
// LOCKTO
void AOBPlayerCameraManager::LockCameraOnPosition(FVector localPos, float deltaTime)
{
    if (!GetCharacter() || !PlayerCamera)
        return;
    if (!GetCharacter()->GetController())
        return;

    const FRotator OldRotation = GetCharacter()->GetController()->GetControlRotation();

    FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(PlayerCamera->GetComponentLocation(), localPos);
    FRotator NewRotation = OldRotation;

    TargetRotation.Roll = OldRotation.Roll;
    TargetRotation.Pitch += GlobalLockOffset.Y;
    TargetRotation.Yaw += GlobalLockOffset.X;
    TargetRotation.Pitch = FMath::Clamp(TargetRotation.Pitch, -CameraLockPitchLowerLimit, CameraLockPitchUpperLimit);

    if (currentLockType == ELockType::EYawOnly)
    {
        TargetRotation.Pitch = OldRotation.Pitch;
    }

    NewRotation = FMath::RInterpTo(OldRotation, TargetRotation, deltaTime, _lockStrength);
    GetCharacter()->GetController()->SetControlRotation(NewRotation);
}

void AOBPlayerCameraManager::LockCameraOnComponent(class USceneComponent* ComponentToLookAt, ELockType lockType, float lockStrength)
{
    if (ComponentToLookAt)
    {
        localtargetComponent = ComponentToLookAt;
        currentLockType = lockType;
        targetLockType = ETargetLockType::EComponent;
        _lockStrength = lockStrength;
    }
}

void AOBPlayerCameraManager::LockCameraOnActor(AActor* ActorLookAt, ELockType lockType, float lockStrength)
{
    if (ActorLookAt)
    {
        localtarget = ActorLookAt;
        currentLockType = lockType;
        targetLockType = ETargetLockType::EActor;
        _lockStrength = lockStrength;
    }
}

void AOBPlayerCameraManager::StopLookingActor()
{
    targetLockType = ETargetLockType::ENone;
}

void AOBPlayerCameraManager::UpdateLockOnTarget(float deltaTime)
{
    if (targetLockType != ETargetLockType::ENone && GetCharacter() && PlayerCamera)
    {
        FVector localPos = FVector();
        if (targetLockType == ETargetLockType::EActor && localtarget)
        {
            localPos = localtarget->GetActorLocation();
        }
        else if (localtargetComponent)
        {
            localPos = localtargetComponent->GetComponentLocation();
        }

        LockCameraOnPosition(localPos, deltaTime);
    }
}

//
//
// SEQUENCE
void AOBPlayerCameraManager::TriggerCameraSequence(AActor* cameraOwner, FName CameraEventName)
{
    if (cameraOwner && !bIsPlayingSequence)
    {
        TArray<UOBCameraSplineComponent*> CameraSequences;
        cameraOwner->GetComponents<UOBCameraSplineComponent>(CameraSequences, true);

        for (UOBCameraSplineComponent* cameraSeq : CameraSequences)
        {
            if (cameraSeq->GetCameraSequenceName() == CameraEventName)
            {
                currentSequence.Reset(cameraSeq);
                bIsPlayingSequence = true;
                break;
            }
        }

        if (currentSequence.cameraSequenceComp)
        {
            SetCameraSequenceData();

            if (UKismetSystemLibrary::IsServer(this))
            {
                UGameplayStatics::SetGlobalTimeDilation(this, currentSequence.cameraSequenceComp->GetCameraSequenceSettings().TimeDilatation);
            }

            APlayerController* controller = UGameplayStatics::GetPlayerController(this, 0);
            if (controller)
            {
                controller->SetViewTargetWithBlend(sequenceCameraActor, currentSequence.cameraSequenceComp->GetCameraSequenceSettings().BlendSettingsTime);
            }
        }
    }
}

void AOBPlayerCameraManager::SetCameraSequenceData()
{
    if (!sequenceCameraActor)
        sequenceCameraActor = GetWorld()->SpawnActor<ACameraActor>(CameraActorClass);

    currentSequenceSettings = currentSequence.cameraSequenceComp->GetCameraSequenceSettings();
    currentSequenceSettings.lookAtActor = currentSequence.cameraSequenceComp->GetLookAtPointLocationByName(currentSequenceSettings.LookAtPoint);

    sequenceCameraActor->GetCameraComponent()->SetFieldOfView(currentSequenceSettings.CameraFov);

    FVector startingLoc = currentSequence.cameraSequenceComp->GetLocationAtTime(0.f, ESplineCoordinateSpace::World);
    FRotator rot = UKismetMathLibrary::FindLookAtRotation(startingLoc, currentSequenceSettings.lookAtActor->GetActorLocation());

    sequenceCameraActor->SetActorLocationAndRotation(startingLoc, rot);
}

void AOBPlayerCameraManager::UpdateCameraSequence(float deltaTime)
{
    if (currentSequence.cameraSequenceComp && currentSequence.currentTime < currentSequence.cameraSequenceComp->GetSplineLength() && currentSequenceSettings.lookAtActor)
    {
        UCameraComponent* cameraComp = sequenceCameraActor->GetCameraComponent();
        if (!cameraComp)
            return;

        const float interpFov = FMath::FInterpTo(cameraComp->FieldOfView, currentSequenceSettings.CameraFov, deltaTime, currentSequenceSettings.FovInterpSpeed);

        cameraComp->SetFieldOfView(interpFov);

        float currentSpeed = currentSequenceSettings.CameraSpeed;
        float const SplineLen = currentSequence.cameraSequenceComp->GetSplineLength();

        currentSequence.currentTime += currentSpeed * deltaTime * SplineLen;
        FVector const MountPos = currentSequence.cameraSequenceComp->GetLocationAtDistanceAlongSpline(currentSequence.currentTime, ESplineCoordinateSpace::World);

        sequenceCameraActor->SetActorLocation(MountPos);
        FRotator lookAtRot = UKismetMathLibrary::FindLookAtRotation(MountPos, currentSequenceSettings.lookAtActor->GetActorLocation());
        FRotator smoothedLookAt = FMath::RInterpTo(sequenceCameraActor->GetActorRotation(), lookAtRot, deltaTime, currentSequenceSettings.CameraRotationsSpeed);

        sequenceCameraActor->SetActorRotation(smoothedLookAt);
    }
    else
    {
        StopCurrentCameraSequence();
    }
}

void AOBPlayerCameraManager::StopCurrentCameraSequence()
{
    bIsPlayingSequence = false;
    if (UKismetSystemLibrary::IsServer(this))
    {
        UGameplayStatics::SetGlobalTimeDilation(this, 1.f);
    }

    APlayerController* controller = UGameplayStatics::GetPlayerController(this, 0);
    if (controller && sequenceCameraActor)
    {
        AActor* newTargeter = Cast<AOBCharacter>(PCOwner->GetPawn());
        controller->SetViewTargetWithBlend(newTargeter, currentSequence.cameraSequenceComp->GetCameraSequenceSettings().OutBlendSettings);
    }
}

//
//
// EVENT
void AOBPlayerCameraManager::TriggerCameraEvent(FName CameraEventName)
{
    if (CameraEventName == NAME_None)
        return;
    if (UKismetSystemLibrary::IsDedicatedServer(this))
        return;

    /*
    if (CameraMovements)
    {
        FCCMCameraMovementSettings* mov = CameraMovements->FindRow<FCCMCameraMovementSettings>(CameraEventName, "Searching for camera event");

        if (currentlyActiveCameraEvents.Find(CameraEventName)) {
            UE_LOG(LogTemp, Warning, TEXT("Camera Event Already Triggered - Cinematic Camera Manager"));
            return;
        }

        if (mov) {
            FinalMov += *(mov);
            currentlyActiveCameraEvents.Add(CameraEventName, *mov);
        }
    }
    */
}

void AOBPlayerCameraManager::StopCameraEvent(FName CameraEventName)
{
    if (CameraEventName == NAME_None) {
        return;
    }

    /*
    if (currentlyActiveCameraEvents.Contains(CameraEventName)) {
        FCCMCameraMovementSettings* mov = currentlyActiveCameraEvents.Find(CameraEventName);
        if (mov) {
            FinalMov -= *(mov);
            currentlyActiveCameraEvents.FindAndRemoveChecked(CameraEventName);
        }
    }
    */
}

void AOBPlayerCameraManager::TriggerTimedCameraEvent(FName CameraEventName, float duration)
{
    if (!UKismetSystemLibrary::IsDedicatedServer(this))
    {
        /*
        if (currentlyActiveCameraEvents.Find(CameraEventName))
        {
            UE_LOG(LogTemp, Warning, TEXT("Camera Event Already Triggered - Cinematic Camera Manager"));
            return;
        }

        UWorld* world = GetWorld();
        if (world) {
            FTimerDelegate TimerDel;
            FTimerHandle TimerHandle;
            TimerDel.BindUFunction(this, FName("StopCameraEvent"), CameraEventName);
            world->GetTimerManager().SetTimer(TimerHandle, TimerDel, duration, false);
            TriggerCameraEvent(CameraEventName);
        }
        */
    }
}

void AOBPlayerCameraManager::StartFading(bool InFadeToBlack)
{
    float FromAlpha = InFadeToBlack ? 0.f : 1.f;
    float ToAlpha = InFadeToBlack ? 1.f : 0.f;
    float Duration = InFadeToBlack ? 0.65f : 0.45f;
    StartCameraFade(FromAlpha, ToAlpha, Duration, FLinearColor::Black, false, true);
}
