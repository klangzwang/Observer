#include "OBStealth.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Components/BoxComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OBStealth)

static FName NAME_StealthCollisionProfile_Box(TEXT("OverlapThePlayer"));

AOBStealth::AOBStealth()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	RootComponent = Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	Box->SetupAttachment(RootComponent);

	/*
	if (Mesh && Box)
	{
		FVector Min, Max;
		Mesh->GetLocalBounds(Min, Max);
		FVector NewExtent = (Max - Min) / 2.0f;
		Box->SetBoxExtent(NewExtent);
		FVector Center = (Max + Min) / 2.0f;
		Box->SetRelativeLocation(Center);
	}
	*/

	Box->SetCollisionProfileName(NAME_StealthCollisionProfile_Box);
	Box->ShapeColor = FColor::Cyan;
	Box->SetOverlayColor(FColor::White);
	Box->SetBoxExtent(FVector(200.f, 200.f, 100.f));
	Box->SetLineThickness(1.f);
	Box->SetHiddenInGame(false);

	Box->OnComponentBeginOverlap.AddDynamic(this, &AOBStealth::OnOverlapBegin);
	Box->OnComponentEndOverlap.AddDynamic(this, &AOBStealth::OnOverlapEnd);

	Tags.Add(FName("HidingBush"));
}

void AOBStealth::BeginPlay()
{
	Super::BeginPlay();
}

void AOBStealth::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (StealthInSound != nullptr)
	{
		UGameplayStatics::SpawnSoundAttached(StealthInSound, Mesh, "none", FVector::ZeroVector, FRotator::ZeroRotator,
			EAttachLocation::SnapToTarget, false, 1.f, 1.f, 0.0f, nullptr, nullptr, true);
	}

	ACharacter* PlayerCharacter = Cast<ACharacter>(OtherActor);
	if (PlayerCharacter)
	{
		if (PlayerCharacter->IsPlayerControlled())
		{
			PlayerCharacter->Tags.Empty();
			PlayerCharacter->Tags.Add(FName("CanBeHidden"));
		}
	}
}

void AOBStealth::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (StealthOutSound != nullptr)
	{
		UGameplayStatics::SpawnSoundAttached(StealthOutSound, Mesh, "none", FVector::ZeroVector, FRotator::ZeroRotator,
			EAttachLocation::SnapToTarget, false, 1.f, 1.f, 0.0f, nullptr, nullptr, true);
	}

	ACharacter* PlayerCharacter = Cast<ACharacter>(OtherActor);
	if (PlayerCharacter)
	{
		if (PlayerCharacter->IsPlayerControlled())
		{
			PlayerCharacter->Tags.Empty();
			PlayerCharacter->Tags.Add(FName("Player"));
		}
	}
}
