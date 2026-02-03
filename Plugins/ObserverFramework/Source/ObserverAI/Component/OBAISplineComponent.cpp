#include "OBAISplineComponent.h"
#include "PrimitiveSceneProxy.h"
#include "OBAIPatrolPath.h"
#include "OBAIPatrolPoint.h"
#include "GameFramework/Actor.h"
#include "UObject/ConstructorHelpers.h"
#include "PrimitiveDrawingUtils.h"
#include "DynamicMeshBuilder.h"
#include "Materials/Material.h"
#include "Materials/MaterialRenderProxy.h"
#include "PrimitiveViewRelevance.h"
#include "Engine/Engine.h"
#include "Engine/CollisionProfile.h"
#include "SceneInterface.h"
#include "SceneView.h"
#include "UObject/UObjectIterator.h"
#include "StaticMeshResources.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OBAISplineComponent)

class FSplineSceneProxy final : public FPrimitiveSceneProxy
{

public:

	SIZE_T GetTypeHash() const override
	{
		static size_t UniquePointer;
		return reinterpret_cast<size_t>(&UniquePointer);
	}

	FSplineSceneProxy(UOBAISplineComponent* Component)
		: FPrimitiveSceneProxy(Component)
		//, VertexFactory(GetScene().GetFeatureLevel(), "FSplineSceneProxy")
		, SplineInfo(Component->SplineCurves.Position)
		, LineColor(FLinearColor::Gray)
		, AISplineData(Component->AISplineData)
	{
		bWillEverBeLit = true;

		/*
		const float HeadAngle = FMath::DegreesToRadians(20.f);
		const float DefaultLength = 1.0f * 80.0f;
		const float TotalLength = 1.0f * 32.f;
		const float HeadLength = DefaultLength * 0.2f;
		const float ShaftRadius = DefaultLength * 0.03f;
		const float ShaftLength = (TotalLength - HeadLength * 0.5);
		const FVector ShaftCenter = FVector(0.5f * ShaftLength, 0, 0);

		TArray<FDynamicMeshVertex> OutVerts;
		BuildConeVerts(HeadAngle, HeadAngle, -HeadLength, TotalLength, 32, OutVerts, IndexBuffer.Indices);
		BuildCylinderVerts(ShaftCenter, FVector(0, 0, 1), FVector(0, 1, 0), FVector(1, 0, 0), ShaftRadius, 0.5f * ShaftLength, 16, OutVerts, IndexBuffer.Indices);

		VertexBuffers.InitFromDynamicVertex(&VertexFactory, OutVerts);

		BeginInitResource(&IndexBuffer);
		*/
	}

	/*
	virtual ~FSplineSceneProxy()
	{
		VertexBuffers.PositionVertexBuffer.ReleaseResource();
		VertexBuffers.StaticMeshVertexBuffer.ReleaseResource();
		VertexBuffers.ColorVertexBuffer.ReleaseResource();
		IndexBuffer.ReleaseResource();
		VertexFactory.ReleaseResource();
	}
	*/

public:

	virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override
	{
		QUICK_SCOPE_CYCLE_COUNTER(STAT_SplineSceneProxy_GetDynamicMeshElements);

		if (IsSelected())
		{
			return;
		}

		for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
		{
			const FSceneView* View = Views[ViewIndex];
			FPrimitiveDrawInterface* PDI = Collector.GetPDI(ViewIndex);

			const FMatrix& LocalWorld = GetLocalToWorld();

			const float DistanceSqr = (View->ViewMatrices.GetViewOrigin() - LocalWorld.GetOrigin()).SizeSquared();
			if (DistanceSqr < FMath::Square(GetMinDrawDistance()) || DistanceSqr > FMath::Square(GetMaxDrawDistance()))
			{
				continue;
			}

			FVector OldKeyPos(0);

			const int32 NumPoints = SplineInfo.Points.Num();
			const int32 NumSegments = SplineInfo.bIsLooped ? NumPoints : NumPoints - 1;

			for (int32 KeyIdx = 0; KeyIdx < NumSegments + 1; KeyIdx++)
			{
				const FVector NewKeyPos = LocalWorld.TransformPosition(SplineInfo.Eval(static_cast<float>(KeyIdx), FVector(0)));

				if (KeyIdx < NumPoints)
				{
					PDI->DrawPoint(NewKeyPos, FColor::White, AISplineData.SplinePointSize + 6, SDPG_World);
					PDI->DrawPoint(NewKeyPos + FVector(0.f, 0.f, 20.f), FColor::Cyan, AISplineData.SplinePointSize, SDPG_World);
				}

				// If not the first keypoint, draw a line to the previous keypoint.
				if (KeyIdx > 0)
				{
					// For constant interpolation - don't draw ticks - just draw dotted line.
					if (SplineInfo.Points[KeyIdx - 1].InterpMode == CIM_Constant)
					{
						// Calculate dash length according to size on screen
						const float StartW = View->WorldToScreen(OldKeyPos).W;
						const float EndW = View->WorldToScreen(NewKeyPos).W;

						const float WLimit = 10.0f;
						if (StartW > WLimit || EndW > WLimit)
						{
							const float Scale = 0.03f;
							DrawDashedLine(PDI, OldKeyPos + FVector(0.f, 0.f, 20.f), NewKeyPos, LineColor, FMath::Max(StartW, EndW) * Scale, SDPG_World);
						}
					}
					else
					{
						// Find position on first keyframe.
						FVector OldPos = OldKeyPos;

						// Then draw a line for each substep.
						const int32 NumSteps = 20;

						for (int32 StepIdx = 1; StepIdx <= NumSteps; StepIdx++)
						{
							const float Key = (KeyIdx - 1) + (StepIdx / static_cast<float>(NumSteps));
							const FVector NewPos = LocalWorld.TransformPosition(SplineInfo.Eval(Key, FVector(0)));
							PDI->DrawTranslucentLine(OldPos + FVector(0.f, 0.f, 20.f), NewPos + FVector(0.f, 0.f, 20.f), LineColor, SDPG_World, 2.f);
							OldPos = NewPos;
						}
					}
				}

				OldKeyPos = NewKeyPos;
			}

			//RenderDynamicMeshElements(Views, ViewFamily, VisibilityMap, Collector);

			/*
			for (int32 SplineKeyIdx = 0; SplineKeyIdx < NumSegments + 1; SplineKeyIdx++)
			{
				if (SplineKeyIdx > 0)
				{
					if (SplineKeyIdx + 1 <= NumPoints)
					{
						const FVector start = LocalWorld.TransformPosition(SplineInfo.Eval(static_cast<float>(SplineKeyIdx), FVector(0))) + FVector(0.f, 0.f, 20.f);
						const FVector end = LocalWorld.TransformPosition(SplineInfo.Eval(static_cast<float>(SplineKeyIdx + 1), FVector(0))) + FVector(0.f, 0.f, 20.f);

						UOBAISplineComponent::DrawRouteLine(InWorld, start, end, FColor::Cyan, 2.5f);
						UOBAISplineComponent::DrawRouteArrow(InWorld, start, end, FColor::Cyan, 2.5f, 50.f, 1300.f, InArrowMoverValue);
					}
				}
			}
			*/

			/*
			if (VisibilityMap & (1 << ViewIndex))
			{
				const FSceneView* View = Views[ViewIndex];
				FPrimitiveDrawInterface* PDI = Collector.GetPDI(ViewIndex);

				const FMatrix& LocalWorld = GetLocalToWorld();

				// Taking into account the min and maximum drawing distance
				const float DistanceSqr = (View->ViewMatrices.GetViewOrigin() - LocalWorld.GetOrigin()).SizeSquared();
				if (DistanceSqr < FMath::Square(GetMinDrawDistance()) || DistanceSqr > FMath::Square(GetMaxDrawDistance()))
				{
					continue;
				}

				UOBAISplineComponent::DrawIt(PDI, View, SplineInfo, LocalWorld, LineColor, SDPG_World);
			}
			*/
		}
	}

	/*
	virtual void RenderDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const
	{
		QUICK_SCOPE_CYCLE_COUNTER(STAT_ArrowSceneProxy_DrawDynamicElements);

		FMatrix EffectiveLocalToWorld = GetLocalToWorld();

		auto SplineAddMaterialRenderProxy = new FColoredMaterialRenderProxy(
			GEngine->ArrowMaterial->GetRenderProxy(),
			AISplineData.SplinePointColor,
			"GizmoColor"
		);

		Collector.RegisterOneFrameMaterialProxy(SplineAddMaterialRenderProxy);

		for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
		{
			if (VisibilityMap & (1 << ViewIndex))
			{
				const FSceneView* View = Views[ViewIndex];

				// Draw the mesh.
				FMeshBatch& Mesh = Collector.AllocateMesh();
				FMeshBatchElement& BatchElement = Mesh.Elements[0];
				BatchElement.IndexBuffer = &IndexBuffer;
				Mesh.bWireframe = false;
				Mesh.VertexFactory = &VertexFactory;
				Mesh.MaterialRenderProxy = SplineAddMaterialRenderProxy;

				FDynamicPrimitiveUniformBuffer& DynamicPrimitiveUniformBuffer = Collector.AllocateOneFrameResource<FDynamicPrimitiveUniformBuffer>();
				DynamicPrimitiveUniformBuffer.Set(Collector.GetRHICommandList(), FScaleMatrix(1.0f) * EffectiveLocalToWorld, FScaleMatrix(1.0f) * EffectiveLocalToWorld, GetBounds(), GetLocalBounds(), true, false, AlwaysHasVelocity());
				BatchElement.PrimitiveUniformBufferResource = &DynamicPrimitiveUniformBuffer.UniformBuffer;

				BatchElement.FirstIndex = 0;
				BatchElement.NumPrimitives = IndexBuffer.Indices.Num() / 3;
				BatchElement.MinVertexIndex = 0;
				BatchElement.MaxVertexIndex = VertexBuffers.PositionVertexBuffer.GetNumVertices() - 1;
				Mesh.ReverseCulling = IsLocalToWorldDeterminantNegative();
				Mesh.Type = PT_TriangleList;
				Mesh.DepthPriorityGroup = SDPG_World;
				Mesh.bCanApplyViewModeOverrides = false;
				Collector.AddMesh(ViewIndex, Mesh);
			}
		}
	}
	*/

	virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override
	{
		FPrimitiveViewRelevance Result;
		Result.bDrawRelevance = true;
		Result.bDynamicRelevance = true;
		Result.bShadowRelevance = IsShadowCast(View);
		Result.bEditorPrimitiveRelevance = UseEditorCompositing(View);
		return Result;
	}

	/*
	virtual void OnTransformChanged(FRHICommandListBase& RHICmdList) override
	{
		Origin = GetLocalToWorld().GetOrigin();
	}
	*/

	virtual uint32 GetMemoryFootprint(void) const override { return sizeof * this + GetAllocatedSize(); }
	uint32 GetAllocatedSize(void) const { return FPrimitiveSceneProxy::GetAllocatedSize(); }

private:

	/*
	FStaticMeshVertexBuffers VertexBuffers;
	FDynamicMeshIndexBuffer32 IndexBuffer;
	FLocalVertexFactory VertexFactory;
	*/

	//FVector Origin;
	FInterpCurveVector SplineInfo;
	FLinearColor LineColor;
	FAISplineData AISplineData;
};

UOBAISplineComponent::UOBAISplineComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

#if WITH_EDITORONLY_DATA

	Icon = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(TEXT("Icon"));

	if (!IsRunningCommandlet())
	{
		struct FConstructorStatics
		{
			ConstructorHelpers::FObjectFinderOptional<UTexture2D> PatrolIconObject;
			FName ID_PatrolPath;
			FText NAME_PatrolPath;

			FConstructorStatics()
				: PatrolIconObject(TEXT("/ObserverFramework/Icons/route"))
				, ID_PatrolPath(TEXT("PatrolPath"))
				, NAME_PatrolPath(NSLOCTEXT("SpriteCategory", "PatrolPath", "Patrol Path"))
			{
			}
		};

		static FConstructorStatics ConstructorStatics;

		if (Icon)
		{
			Icon->Sprite = ConstructorStatics.PatrolIconObject.Get();
			Icon->SetRelativeScale3D_Direct(FVector(0.25f, 0.25f, 0.25f));
			Icon->SetRelativeLocation_Direct(FVector(0.f, 0.f, 10.f));
			Icon->SpriteInfo.Category = ConstructorStatics.ID_PatrolPath;
			Icon->SpriteInfo.DisplayName = ConstructorStatics.NAME_PatrolPath;
			Icon->bIsScreenSizeScaled = true;
			//Icon->SetupAttachment(RootComponent);
		}
	}

#endif

	SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	SetGenerateOverlapEvents(false);

#if WITH_EDITORONLY_DATA
	EditorUnselectedSplineSegmentColor = FLinearColor::Blue;
	EditorSelectedSplineSegmentColor = FLinearColor::Yellow;
	EditorTangentColor = FLinearColor::Green;
#endif
}

#if WITH_EDITORONLY_DATA

UBillboardComponent* UOBAISplineComponent::GetIcon() const
{
	return Icon;
}

#endif

FPrimitiveSceneProxy* UOBAISplineComponent::CreateSceneProxy()
{
	Super::CreateSceneProxy();
	return new FSplineSceneProxy(this);
}

void UOBAISplineComponent::BeginPlay()
{
	Super::BeginPlay();

	AOBAIPatrolPath* PathActor = Cast<AOBAIPatrolPath>(GetOwner());
	if (PathActor)
	{
		const int32 NumPoints = SplineCurves.Position.Points.Num();
		const int32 NumSegments = SplineCurves.Position.bIsLooped ? NumPoints : NumPoints - 1;

		for (int32 KeyIdx = 0; KeyIdx < NumSegments + 1; KeyIdx++)
		{
			const FVector NewKeyPos = SplineCurves.Position.Eval(static_cast<float>(KeyIdx), FVector(0));
			PathActor->K2_OnGeneratePathPoints(NewKeyPos);
		}
	}
}

FVector UOBAISplineComponent::GetNextWaypointLocation(float DistanceAlongSpline)
{
	return GetLocationAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::World);
}

void UOBAISplineComponent::SetBoundToPath(bool IsBounded)
{
	if(bIsBound != IsBounded)
		bIsBound = IsBounded;
}

/*
void UOBAISplineComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
*/
