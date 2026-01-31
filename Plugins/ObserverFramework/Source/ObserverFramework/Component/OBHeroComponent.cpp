#include "OBHeroComponent.h"
#include "EnhancedInputSubsystems.h"
#include "OBPlayerController.h"
#include "OBPlayerCameraManager.h"
#include "OBPlayerState.h"
#include "OBLocalPlayer.h"
#include "OBCharacter.h"
#include "OBInputConfig.h"
#include "OBInputComponent.h"
#include "OBCharacterMovementComponent.h"
#include "OBGameplayTags.h"
#include "OBGameState.h"
#include "OBSystemStatics.h"
#include "Components/PostProcessComponent.h"
#include "Components/GameFrameworkComponentManager.h"
#include "PlayerMappableInputConfig.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "InputMappingContext.h"
#include "Engine/Canvas.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AIPerceptionComponent.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "CineCameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OBHeroComponent)

#if WITH_EDITOR
#include "Misc/UObjectToken.h"
#endif

namespace ObserverHero
{
	static const float LookYawRate = 300.0f;
	static const float LookPitchRate = 165.0f;
};

struct FCharInputState;

FActionInstance::FActionInstance(const FInputActionInstance InInputActionInstance)
	: InputActionInstance(InInputActionInstance)
{
}

UOBHeroComponent::UOBHeroComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	postProcessComponent = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcessComponent"));
	if (postProcessComponent != nullptr)
		postProcessComponent->bUnbound = true;

	OnCrouchMontageEnded.BindUObject(this, &UOBHeroComponent::CrouchingMontageEnded);

	Flag = EInputFlags::INP_None;

	bReadyToBindInputs = false;
	InputConfig = nullptr;

	MovingNoiseFrames = 0;
}

void UOBHeroComponent::OnRegister()
{
	Super::OnRegister();

	if (!GetPawn<APawn>())
	{
		UE_LOG(LogTemp, Error, TEXT("[UOBHeroComponent::OnRegister] This component has been added to a blueprint whose base class is not a Pawn. To use this component, it MUST be placed on a Pawn Blueprint."));

#if WITH_EDITOR
		if (GIsEditor)
		{
			static const FText Message = NSLOCTEXT("OBHeroComponent", "NotOnPawnError", "has been added to a blueprint whose base class is not a Pawn. To use this component, it MUST be placed on a Pawn Blueprint. This will cause a crash if you PIE!");
			static const FName HeroMessageLogName = TEXT("OBHeroComponent");
			
			FMessageLog(HeroMessageLogName).Error()
				->AddToken(FUObjectToken::Create(this, FText::FromString(GetNameSafe(this))))
				->AddToken(FTextToken::Create(Message));
				
			FMessageLog(HeroMessageLogName).Open();
		}
#endif
	}
}

void UOBHeroComponent::BeginPlay()
{
	Super::BeginPlay();

	if (postProcessComponent != nullptr)
	{
		dynamicMaterial = UMaterialInstanceDynamic::Create(outlineMaterial, GetWorld(), FName(TEXT("OutlineMaterial")));
		postProcessComponent->Settings.WeightedBlendables.Array.Init(FWeightedBlendable(), 1);
		postProcessComponent->Settings.WeightedBlendables.Array[0].Object = dynamicMaterial;
		postProcessComponent->Settings.WeightedBlendables.Array[0].Weight = 1.0f;
	}
	if (dynamicMaterial != nullptr)
	{
		//dynamicMaterial->SetScalarParameterValue(TEXT("FirstStencilValue"), firstStencilValue);
		//dynamicMaterial->SetScalarParameterValue(TEXT("LastStencilValue"), lastStencilValue);
		//dynamicMaterial->SetScalarParameterValue(TEXT("LineThickness"), lineThickness);
	}
}

void UOBHeroComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void UOBHeroComponent::DispDebug(UCanvas* Canvas, float YL, float YPos)
{
	if (GetCharacter() == NULL)
		return;

	Canvas->SetDrawColor(255, 255, 255);
	UFont* RenderFont = GEngine->GetSmallFont();
	FString T = FString::Printf(TEXT(""));

	if (mInputUsingGate.IsAiming)
	{
		T = FString::Printf(TEXT("AIMS"));
	}
	else if (mInputUsingGate.IsAttacking)
	{
		T = FString::Printf(TEXT("ATTACKING %f"), GetCharacter()->GetWorld()->GetTimeSeconds());
	}

	Canvas->DrawText(RenderFont, T, 4.0f, YPos);
	YPos += YL;
	T = FString::Printf(TEXT("AVERAGE SPEED %f"), MaxWalkSpeedDefault);
	Canvas->DrawText(RenderFont, T, 4.0f, YPos);
	YPos += YL;
}

void UOBHeroComponent::SetInputUsingGate(FInputUsingGate InInputUsingGate)
{
	mInputUsingGate.IsVaulting = InInputUsingGate.IsVaulting;
	mInputUsingGate.IsMoving = InInputUsingGate.IsMoving;
	mInputUsingGate.IsLooking = InInputUsingGate.IsLooking;
	mInputUsingGate.IsCrouching = InInputUsingGate.IsCrouching;
	mInputUsingGate.IsSprinting = InInputUsingGate.IsSprinting;
	mInputUsingGate.IsSliding = InInputUsingGate.IsSliding;
	mInputUsingGate.IsDodging = InInputUsingGate.IsDodging;
	mInputUsingGate.IsAiming = InInputUsingGate.IsAiming;
	mInputUsingGate.IsAttacking = InInputUsingGate.IsAttacking;
	mInputUsingGate.IsThrowing = InInputUsingGate.IsThrowing;
	mInputUsingGate.IsUsing = InInputUsingGate.IsUsing;
	mInputUsingGate.IsRolling = InInputUsingGate.IsRolling;
	mInputUsingGate.IsBlocking = InInputUsingGate.IsBlocking;
	mInputUsingGate.IsHiding = InInputUsingGate.IsHiding;
	mInputUsingGate.IsStealth = InInputUsingGate.IsStealth;
	mInputUsingGateDelegate.Broadcast(InInputUsingGate);
}

void UOBHeroComponent::CrouchingMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	FInputUsingGate Input = GetUsingInputGate();

	if (!Input.IsMoving)
	{
		GetCharacter()->Crouch();

		Input.IsCrouching = true;
		Input.IsRolling = false;
		Input.IsSliding = false;

		SetInputUsingGate(Input);
	}
}

void UOBHeroComponent::InitializeComponent()
{
	Super::InitializeComponent();

	FInputUsingGate inputUsing;
	inputUsing.IsVaulting = false;
	inputUsing.IsMoving = false;
	inputUsing.IsLooking = false;
	inputUsing.IsCrouching = false;
	inputUsing.IsSprinting = false;
	inputUsing.IsSliding = false;
	inputUsing.IsDodging = false;
	inputUsing.IsAiming = false;
	inputUsing.IsAttacking = false;
	inputUsing.IsThrowing = false;
	inputUsing.IsUsing = false;
	inputUsing.IsRolling = false;
	inputUsing.IsBlocking = false;
	inputUsing.IsHiding = false;
	inputUsing.IsStealth = false;
	SetInputUsingGate(inputUsing);
}

void UOBHeroComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
		return;

	const APlayerController* PC = GetController<APlayerController>();
	check(PC);

	const AOBCharacter* CH = Cast<AOBCharacter>(GetOwner());
	check(CH);

	const UOBLocalPlayer* LP = Cast<UOBLocalPlayer>(PC->GetLocalPlayer());
	check(LP);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	Subsystem->ClearAllMappings();

	if (InputConfig)
	{
		int32 Priority = InputMappingContexts.Num();

		for (const TObjectPtr<const UInputMappingContext>& Imc : InputMappingContexts)
		{
			if (ensure(Imc != nullptr))
			{
				FModifyContextOptions Options = {};
				Options.bIgnoreAllPressedKeysUntilRelease = false;

				Subsystem->AddMappingContext(Imc, Priority, Options);

				Priority--;
			}
		}

		for (const FOBInputAction& MovementAction : InputConfig->NativeInputActions)
		{
			//UEnhancedPlayerInput* EnhancedPlayerInput = Cast<UEnhancedPlayerInput>(PC->PlayerInput);
			//EnhancedPlayerInput->FindActionInstanceData(MovementAction.InputAction)->GetTriggerEvent();

			if (MovementAction.InputTag.IsValid())
			{
				UOBInputComponent* ObserverIC = Cast<UOBInputComponent>(PlayerInputComponent);
				if (ensureMsgf(ObserverIC, TEXT("Unexpected Input Component class!")))
				{
					ObserverIC->AddInputMappings(InputConfig, Subsystem);

					TArray<uint32> BindHandles;
					ObserverIC->BindAbilityActions(InputConfig, this, &UOBHeroComponent::Input_AbilityInputTagPressed, &UOBHeroComponent::Input_AbilityInputTagReleased, BindHandles);

					UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
					if (ensure(EnhancedInput))
					{
						ObserverIC->BindNativeActionByTag(EnhancedInput, InputConfig, MovementAction.InputTag, ETriggerEvent::Started, this, &UOBHeroComponent::MovementRouting);
						ObserverIC->BindNativeActionByTag(EnhancedInput, InputConfig, MovementAction.InputTag, ETriggerEvent::Triggered, this, &UOBHeroComponent::MovementRouting);
						ObserverIC->BindNativeActionByTag(EnhancedInput, InputConfig, MovementAction.InputTag, ETriggerEvent::Completed, this, &UOBHeroComponent::MovementRouting);
						ObserverIC->BindNativeActionByTag(EnhancedInput, InputConfig, MovementAction.InputTag, ETriggerEvent::None, this, &UOBHeroComponent::MovementRouting);
					}
				}
			}
		}
	}

	if (ensure(!bReadyToBindInputs))
	{
		bReadyToBindInputs = true;
	}
}

bool UOBHeroComponent::IsReadyToBindInputs() const
{
	return bReadyToBindInputs;
}

void UOBHeroComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (const APawn* Pawn = GetPawn<APawn>())
	{
		// Implement a Component for example, to use this!
	}
}

void UOBHeroComponent::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	// Implement a Component for example, to use this!
}

void UOBHeroComponent::MovementRouting(const FInputActionInstance& Instance)
{
	UWorld* World = GetWorld();

	AOBGameState* GameState = Cast<AOBGameState>(World->GetGameState());
	UOBCharacterMovementComponent* MoveComp = Cast<UOBCharacterMovementComponent>(GetCharacter()->GetCharacterMovement());
	AOBPlayerCameraManager* CameraManager = Cast<AOBPlayerCameraManager>(UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0));

	if (!ensure(GameState) || !ensure(MoveComp) || !ensure(CameraManager))
		return;

	FString ActionName = Instance.GetSourceAction()->GetName();

	FVector VectorValue = Instance.GetValue().Get<FVector>();
	FVector2D AxisValue = Instance.GetValue().Get<FVector2D>();
	float FloatValue = Instance.GetValue().Get<float>();
	bool BoolValue = Instance.GetValue().Get<bool>();

	float ElapsedTime = Instance.GetElapsedTime();

	FInputUsingGate Input = GetUsingInputGate();

	if (GetCharacter()->Controller != nullptr)
	{
		switch (Instance.GetTriggerEvent())
		{
			case ETriggerEvent::Triggered:

				//
				// MOVE
				//
				if (ActionName == "IA_Move")
				{
					if (!GetCharacter()->bIsCrouched)
					{
						if (MovingNoiseFrames > 0)
							MovingNoiseFrames--;
						else
						{
							MovingNoiseFrames = 12;

							UAISense_Hearing::ReportNoiseEvent(
								GetWorld(),
								GetCharacter()->GetActorLocation(),
								1.f,
								GetCharacter(),
								0.f,
								FName("MovingNoise")
							);
						}
					}

					const FRotator Rotation = GetCharacter()->Controller->GetControlRotation();
					const FRotator YawRotation(0, Rotation.Yaw, 0);
					const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
					const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
					MoveComp->AddInputVector(ForwardDirection * AxisValue.Y, false);
					MoveComp->AddInputVector(RightDirection * AxisValue.X, false);

					Input.IsMoving = true;

					if (MoveComp->Velocity.Length() > 0.2f)
					{
						if (AxisValue.Y > 0.01f)
						{
							Flag = EInputFlags::INP_Forward;

							if (AxisValue.X > 0.01f)
								Flag = EInputFlags::INP_ForwardRight;
							else if (AxisValue.X < -0.01f)
								Flag = EInputFlags::INP_ForwardLeft;
						}
						else if (AxisValue.Y < -0.01f)
						{
							Flag = EInputFlags::INP_Backward;

							if (AxisValue.X > 0.01f)
								Flag = EInputFlags::INP_BackwardRight;
							else if (AxisValue.X < -0.01f)
								Flag = EInputFlags::INP_BackwardLeft;
						}
						else if (AxisValue.X > 0.01f)
						{
							Flag = EInputFlags::INP_Right;

							if (AxisValue.Y > 0.01f)
								Flag = EInputFlags::INP_ForwardRight;
							else if (AxisValue.Y < -0.01f)
								Flag = EInputFlags::INP_BackwardRight;
						}
						else if (AxisValue.X < 0.01f)
						{
							Flag = EInputFlags::INP_Left;

							if (AxisValue.Y > 0.01f)
								Flag = EInputFlags::INP_ForwardLeft;
							else if (AxisValue.Y < -0.01f)
								Flag = EInputFlags::INP_BackwardLeft;
						}
					}
				}

				//
				// LOOK
				//
				if (ActionName == "IA_Look")
				{
					GetCharacter()->AddControllerYawInput(AxisValue.X);
					GetCharacter()->AddControllerPitchInput(AxisValue.Y);
				}

				//
				// SPRINT
				//
				if (ActionName == "IA_Sprint")
				{
					Input.IsSprinting =
						Flag == EInputFlags::INP_Forward ||
						Flag == EInputFlags::INP_ForwardLeft ||
						Flag == EInputFlags::INP_ForwardRight;
				}

				//
				//
				//
				if (ActionName != "")
					SetInputUsingGate(Input);

				break;

			case ETriggerEvent::Started:

				//
				// USE
				//
				if (ActionName == "IA_Use")
				{
					Input.IsUsing = true;
					GetCharacter()->PerformInteractionCheck();
				}

				//
				// ATTACK
				//
				if (ActionName == "IA_Attack")
				{
					/*
					GetCharacter()->PlayAnimMontage(
						GetCharacter()->GetWeapon()->NeedsReload() ?
						GetCharacter()->GetWeapon()->WeaponDef.ReloadMontage :
						!Input.IsAiming ?
						GetCharacter()->GetWeapon()->WeaponDef.MeleeMontage :
						GetCharacter()->GetWeapon()->WeaponDef.FiringMontage);
					*/

					Input.IsAttacking = true;
				}

				//
				// AIM
				//
				if (ActionName == "IA_Aim")
				{
					//MaxWalkSpeedDefault = MaxWalkSpeed;
					//MaxWalkSpeed = MaxWalkSpeedDefault * 0.25f;

					//GetCharacter()->K2_OnStartAim();

					Input.IsAiming = true;

					CameraManager->SetCamera(FVector(20.f, 0.f, 0.f), 10.f);
				}

				//
				// CROUCH
				//
				if (ActionName == "IA_Crouch")
				{
					if (UAnimInstance* AnimInstance = GetCharacter()->GetMesh()->GetAnimInstance())
					{
						UAnimMontage* Montage = nullptr;

						if (Flag == EInputFlags::INP_Forward)
							Montage = Input.IsSprinting ? CharacterClass.SlideForward : CharacterClass.RollForward;
						else if (Flag == EInputFlags::INP_Backward)
							Montage = CharacterClass.RollBackward;
						else if (Flag == EInputFlags::INP_Left)
							Montage = CharacterClass.RollLeft;
						else if (Flag == EInputFlags::INP_Right)
							Montage = CharacterClass.RollRight;
						else if (Flag == EInputFlags::INP_ForwardLeft)
							Montage = Input.IsSprinting ? CharacterClass.SlideForward : CharacterClass.RollForward;
						else if (Flag == EInputFlags::INP_ForwardRight)
							Montage = Input.IsSprinting ? CharacterClass.SlideForward : CharacterClass.RollForward;
						else if (Flag == EInputFlags::INP_BackwardLeft)
							Montage = CharacterClass.RollBackward;
						else if (Flag == EInputFlags::INP_BackwardRight)
							Montage = CharacterClass.RollBackward;

						if (Montage != nullptr)
						{
							const float MontageLength = AnimInstance->Montage_Play(Montage, 1.0f, EMontagePlayReturnType::MontageLength, 0.0f, true);
							if (MontageLength > 0.0f)
							{
								AnimInstance->Montage_SetEndDelegate(OnCrouchMontageEnded, Montage);

								Input.IsSliding = Montage == CharacterClass.SlideForward;
								Input.IsRolling =
									Montage == CharacterClass.RollForward ||
									Montage == CharacterClass.RollBackward ||
									Montage == CharacterClass.RollLeft ||
									Montage == CharacterClass.RollRight;
							}
						}
						else
						{
							if (GetCharacter()->bIsCrouched || MoveComp->bWantsToCrouch)
							{
								GetCharacter()->UnCrouch();
								Input.IsCrouching = false;
								//GetCharacter()->K2_OnShowOutline(false);
								CameraManager->SetCamera(FVector(0.f, 0.f, 10.f));
							}
							else
							{
								GetCharacter()->Crouch();
								Input.IsCrouching = true;
								//GetCharacter()->K2_OnShowOutline(true);
								CameraManager->SetCamera(FVector(0.f, 0.f, -10.f));
							}
						}
					}

					//
					// StealthSpecific
					//

					// Update InputGate before
					SetInputUsingGate(Input);

					if (Input.IsCrouching)
					{
						if (GetCharacter()->ActorHasTag("CanBeHidden"))
						{
							GetCharacter()->Tags.Empty();
							GetCharacter()->Tags.Add(FName("Hidden"));
							GetCharacter()->SetIsVisible(false);
						}
					}
					else
					{
						if (GetCharacter()->ActorHasTag("Hidden"))
						{
							GetCharacter()->Tags.Empty();
							GetCharacter()->Tags.Add(FName("CanBeHidden"));
							GetCharacter()->SetIsVisible(true);
						}
					}

					UOBSystemStatics::SetRenderCustomDepthOnAllMeshComponents(GetCharacter(), "Outline", GetCharacter()->ActorHasTag("Hidden"));
				}

				//
				//
				//
				if (ActionName != "")
					SetInputUsingGate(Input);

				break;

			case ETriggerEvent::Completed:

				if (ActionName == "IA_Use")
				{
					Input.IsUsing = false;
				}

				if (ActionName == "IA_Move")
				{
					Input.IsMoving = false;

					Flag = EInputFlags::INP_Neutral;
				}

				if (ActionName == "IA_Sprint")
				{
					Input.IsSprinting = false;
				}

				if (ActionName == "IA_Aim")
				{
					Input.IsAiming = false;

					//GetCharacter()->K2_OnEndAim();

					CameraManager->SetCamera(FVector(-20.f, 0.f, 0.f), -10.f);
				}

				if (ActionName == "IA_Attack")
				{
					Input.IsAttacking = false;
				}

				//
				//
				//
				if (ActionName != "")
				{
					MovingNoiseFrames = 12;
					SetInputUsingGate(Input);
				}

				break;

			case ETriggerEvent::None:

				break;
		}
	}
}

void UOBHeroComponent::AddAdditionalInputConfig(const UOBInputConfig* mInputConfig)
{
	TArray<uint32> BindHandles;

	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	const APlayerController* PC = GetController<APlayerController>();
	check(PC);

	const ULocalPlayer* LP = PC->GetLocalPlayer();
	check(LP);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	UOBInputComponent* ObserverIC = Pawn->FindComponentByClass<UOBInputComponent>();
	if (ensureMsgf(ObserverIC, TEXT("Unexpected Input Component class!")))
	{
		ObserverIC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, BindHandles);
	}
}
