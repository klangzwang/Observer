#include "OBAIPatrolPoint.h"
#include "Async/TaskGraphInterfaces.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/ArrowComponent.h"
#include "Components/BillboardComponent.h"
#include "Engine/Texture2D.h"
#include "GameFramework/Character.h"
#include "OBAIComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OBAIPatrolPoint)

AOBAIPatrolPoint::AOBAIPatrolPoint(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("Root")));

	Arrow = CreateEditorOnlyDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	if (Arrow)
	{
		Arrow->SetRelativeLocation(FVector(0.f, 0.f, 50.f));
		Arrow->ArrowColor = FColor::Magenta;
		Arrow->ArrowSize = 2.f;
		Arrow->bTreatAsASprite = true;
		Arrow->SetupAttachment(RootComponent);
		Arrow->bIsScreenSizeScaled = true;
	}

#if WITH_EDITORONLY_DATA

	Icon = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(TEXT("Icon"));

	if (!IsRunningCommandlet())
	{
		struct FConstructorStatics
		{
			ConstructorHelpers::FObjectFinderOptional<UTexture2D> PatrolIconObject;
			FName ID_PatrolPoint;
			FText NAME_PatrolPoint;

			FConstructorStatics()
				: PatrolIconObject(TEXT("/ObserverFramework/Icons/pin"))
				, ID_PatrolPoint(TEXT("PatrolPoint"))
				, NAME_PatrolPoint(NSLOCTEXT("SpriteCategory", "PatrolPoint", "Patrol Points"))
			{
			}
		};

		static FConstructorStatics ConstructorStatics;

		if (Icon)
		{
			Icon->Sprite = ConstructorStatics.PatrolIconObject.Get();
			Icon->SetRelativeScale3D_Direct(FVector(0.25f, 0.25f, 0.25f));
			Icon->SetRelativeLocation_Direct(FVector(0.f, 0.f, 10.f));
			Icon->SpriteInfo.Category = ConstructorStatics.ID_PatrolPoint;
			Icon->SpriteInfo.DisplayName = ConstructorStatics.NAME_PatrolPoint;
			Icon->bIsScreenSizeScaled = true;
			Icon->SetHiddenInGame(false);
			Icon->SetupAttachment(RootComponent);
		}
	}

#endif

	//SetHidden(true);
	SetCanBeDamaged(false);

	Tags.Add("PatrolPoint");
}

#if WITH_EDITORONLY_DATA

UBillboardComponent* AOBAIPatrolPoint::GetIcon() const
{
	return Icon;
}

#endif

UArrowComponent* AOBAIPatrolPoint::GetArrow()
{
	return Arrow;
}

void AOBAIPatrolPoint::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	Arrow->SetWorldRotation(FRotator(0.f, Arrow->GetComponentRotation().Yaw, 0.f).Quaternion());
}
