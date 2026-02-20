#include "OBDeviceBase.h"
#include "OBDeviceComponent.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OBDeviceBase)

AOBDeviceBase::AOBDeviceBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.1f;
	PrimaryActorTick.bStartWithTickEnabled = false;

	Capsule = ObjectInitializer.CreateDefaultSubobject<UCapsuleComponent>(this, TEXT("Capsule"));
	Capsule->InitCapsuleSize(92.f, 134.0f);
	Capsule->SetHiddenInGame(false);
	Capsule->ShapeColor = FColor::Magenta;
	Capsule->SetupAttachment(RootComponent);

	CurrentProgress = 0.f;

	DeviceName = "";
}

void AOBDeviceBase::RegisterDevice(AOBDeviceBase* DeviceActor)
{
	if (DeviceName == "")
	{
		UE_LOG(LogTemp, Warning, TEXT("Device [%s] not registered."), *GetNameSafe(DeviceActor));
		return;
	}

	UWorld* World = GetWorld();
	if (World && GetLocalRole() == ROLE_Authority)
	{
		AGameStateBase* GameState = World->GetGameState<AGameStateBase>();
		if (GameState)
		{
			UOBDeviceComponent* DeviceManager = GameState->FindComponentByClass<UOBDeviceComponent>();
			if (DeviceManager)
			{
				DeviceManager->Devices.Add(DeviceActor);
			}
		}
	}
}

void AOBDeviceBase::BeginPlay()
{
	Super::BeginPlay();
}

void AOBDeviceBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (Character)
	{
		OnOverlapBeginEnd(Character, true);
	}
}

void AOBDeviceBase::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (Character)
	{
		OnOverlapBeginEnd(Character, false);
	}
}

// GEngine->AddOnScreenDebugMessage(111, 4.f, FColor::White, FString::Printf(TEXT("OtherActor: %s"), *OtherActor->GetName()));

/*
UFUNCTION(BlueprintCallable, Category = "Observer|Device")
virtual uint8 GetTeamNumFor(ACharacter* InCharacter) const
{
	if (InCharacter->GetClass()->ImplementsInterface(UGenericTeamAgentInterface::StaticClass()))
	{
		return FGenericTeamId::GetTeamIdentifier(InCharacter).GetId();
	}
	return 0;
}
*/