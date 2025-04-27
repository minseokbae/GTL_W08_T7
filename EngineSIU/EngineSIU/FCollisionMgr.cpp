#include "FCollisionMgr.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/ShapeComponent.h"
#include "UObject/Casts.h"

#include "CollisionHelpers.h"

namespace CollisionChecks
{
    bool CheckSphereSphereOverlap(const USphereComponent* SphereA, const USphereComponent* SphereB);
    bool CheckBoxBoxOverlap(const UBoxComponent* BoxA, const UBoxComponent* BoxB);
    bool CheckSphereBoxOverlap(const USphereComponent* Sphere, const UBoxComponent* Box);
    //bool CheckCapsuleCapsuleOverlap(const UCapsuleComponent* CapsuleA, const UCapsuleComponent* CapsuleB);
    //bool CheckCapsuleSphereOverlap(const UCapsuleComponent* Capsule, const USphereComponent* Sphere);
    //bool CheckCapsuleBoxOverlap(const UCapsuleComponent* Capsule, const UBoxComponent* Box);
    //bool CheckCapsuleCapsuleOverlap(const UCapsuleComponent* CapsuleA, const UCapsuleComponent* CapsuleB);
    //bool CheckBoxBoxOverlap(const UBoxComponent* BoxA, const UBoxComponent* BoxB);
    //bool CheckCapsuleSphereOverlap(const UCapsuleComponent* Capsule, const USphereComponent* Sphere);
    //bool CheckCapsuleBoxOverlap(const UCapsuleComponent* Capsule, const UBoxComponent* Box);
    //bool CheckSphereBoxOverlap(const USphereComponent* Sphere, const UBoxComponent* Box);
} 

FCollisionMgr::FCollisionMgr()
{
}

FCollisionMgr::~FCollisionMgr()
{
}

void FCollisionMgr::RegisterComponent(UShapeComponent* Component)
{
    if (Component != nullptr)
    {
        if (!RegisteredComponents.Contains(Component))
        {
            RegisteredComponents.Add(Component);
        }
    }
}

void FCollisionMgr::UnregisterComponent(UShapeComponent* Component)
{
    if (Component != nullptr)
    {
        int32 RemovedCount = RegisteredComponents.RemoveSingle(Component); // 순서 무관 시 효율적
        //RemoveSingleSwap
        if (RemovedCount > 0)
        {
            // 이 컴포넌트와 관련된 이전 오버랩 상태도 정리해야 함
            RemoveComponentOverlapState(Component);
        }
    }
}

void FCollisionMgr::UpdateCollisionChecks()
{
    TSet<FOverlapPair> CurrentOverlappingPairs;

    // 등록된 모든 컴포넌트 쌍에 대해 검사 (N^2 방식, 개선 필요 시 공간 분할 등 사용)
    for (int32 i = 0; i < RegisteredComponents.Num(); ++i)
    {
        UShapeComponent* CompA = RegisteredComponents[i];
        if (!CompA) continue;

        for (int32 j = i + 1; j < RegisteredComponents.Num(); ++j)
        {
            UShapeComponent* CompB = RegisteredComponents[j];
            if (!CompB) continue; 

            if (IsOverlapping(CompA, CompB))
            {
                // 오버랩 발생
                FOverlapPair CurrentPair(CompA, CompB);
                CurrentOverlappingPairs.Add(CurrentPair);

                if (!PreviousOverlappingPairs.Contains(CurrentPair))
                {
                    // 오버랩 시작
                    AActor* ActorA = CompA->GetOwner();
                    AActor* ActorB = CompB->GetOwner();

                    // 각 컴포넌트의 Notify 함수 호출
                    if (CompA) CompA->NotifyBeginOverlap(CompA, ActorB, CompB);
                    if (CompB) CompB->NotifyBeginOverlap(CompB, ActorA, CompA);
                }
            }
        }
    }

    // 이전 프레임에 오버랩했지만, 이번 프레임에는 오버랩하지 않은 쌍 찾기 (EndOverlap 감지)
    for (const FOverlapPair& PrevPair : PreviousOverlappingPairs)
    {
        // PrevPair의 컴포넌트들이 여전히 유효한지 확인하는 것이 더 안전함
        UShapeComponent* CompA = PrevPair.CompA;
        UShapeComponent* CompB = PrevPair.CompB;
        if (!CompA || !CompB || !RegisteredComponents.Contains(CompA) || !RegisteredComponents.Contains(CompB))
        {
            continue;
        }

        if (!CurrentOverlappingPairs.Contains(PrevPair))
        {
            // 오버랩 종료
            AActor* ActorA = CompA->GetOwner();
            AActor* ActorB = CompB->GetOwner();

            // 각 컴포넌트의 Notify 함수 호출
            if (CompA) CompA->NotifyEndOverlap(CompA, ActorB, CompB);
            if (CompB) CompB->NotifyEndOverlap(CompB, ActorA, CompA);
        }
    }

    // 다음 프레임을 위해 현재 오버랩 상태를 이전 상태로 업데이트
    PreviousOverlappingPairs = std::move(CurrentOverlappingPairs);
}

bool FCollisionMgr::IsOverlapping(UShapeComponent* CompA, UShapeComponent* CompB)
{
    // 0. 유효성 및 기본 검사
    if (!CompA || !CompB || CompA == CompB)
    {
        return false; // Null 이거나 자기 자신과는 오버랩하지 않음
    }

    // UShapeComponent 로 캐스팅 시도
    // Primitive를 가져오는게 맞나..? 아직 고민중임.
    //auto* ShapeA = Cast<UShapeComponent>(CompA);
    //auto* ShapeB = Cast<UShapeComponent>(CompB);    
    auto* ShapeA = CompA;
    auto* ShapeB = CompB;

    if (!ShapeA || !ShapeB)
    {
        return false;
    }

    if (auto* SphereA = Cast<USphereComponent>(CompA))
    {
        //if (auto* CapsuleB = Cast<UCapsuleComponent>(CompB))
        //{
        //    return CollisionChecks::CheckCapsuleSphereOverlap(CapsuleB, SphereA); 
        //}
        if (auto* SphereB = Cast<USphereComponent>(CompB))
        {
            return CollisionChecks::CheckSphereSphereOverlap(SphereA, SphereB);
        }
        if (auto* BoxB = Cast<UBoxComponent>(CompB))
        {
            return CollisionChecks::CheckSphereBoxOverlap(SphereA, BoxB);
        }
        //if (auto* BoxB = Cast<UBoxComponent>(CompB))
        //{
        //    return CollisionChecks::CheckSphereBoxOverlap(SphereA, BoxB);
        //}
    }
    //else if (auto CapsuleA = Cast<UCapsuleComponent>(CompA))
    //{
    //    if (auto* CapsuleB = Cast<UCapsuleComponent>(CompB))
    //    {
    //        return CollisionChecks::CheckCapsuleCapsuleOverlap(CapsuleA, CapsuleB);
    //    }
    //    if (auto* SphereB = Cast<USphereComponent>(CompB))
    //    {
    //        return CollisionChecks::CheckCapsuleSphereOverlap(CapsuleA, SphereB);
    //    }
    //    if (auto* BoxB = Cast<UBoxComponent>(CompB))
    //    {
    //        return CollisionChecks::CheckCapsuleBoxOverlap(CapsuleA, BoxB);
    //    }
    //}
    else if (auto* BoxA = Cast<UBoxComponent>(CompA))
    {
        //if (auto* CapsuleB = Cast<UCapsuleComponent>(CompB))
        //{
        //    return CollisionChecks::CheckCapsuleBoxOverlap(CapsuleB, BoxA); 
        //}
        if (auto* SphereB = Cast<USphereComponent>(CompB))
        {
            return CollisionChecks::CheckSphereBoxOverlap(SphereB, BoxA);
        }
        if (auto* BoxB = Cast<UBoxComponent>(CompB))
        {
            return CollisionChecks::CheckBoxBoxOverlap(BoxA, BoxB);
        }
    }
    //else if (auto* BoxA = Cast<UBoxComponent>(CompA))
    //{
    //    if (auto* CapsuleB = Cast<UCapsuleComponent>(CompB))
    //    {
    //        return CollisionChecks::CheckCapsuleBoxOverlap(CapsuleB, BoxA); 
    //    }
    //    if (auto* SphereB = Cast<USphereComponent>(CompB))
    //    {
    //        return CollisionChecks::CheckSphereBoxOverlap(SphereB, BoxA);
    //    }
    //    if (auto* BoxB = Cast<UBoxComponent>(CompB))
    //    {
    //        return CollisionChecks::CheckBoxBoxOverlap(BoxA, BoxB);
    //    }
    //}

    return false;
}

void FCollisionMgr::RemoveComponentOverlapState(UShapeComponent* Component)
{
    // PreviousOverlappingPairs에서 제거된 컴포넌트를 포함하는 모든 쌍을 제거
    TSet<FOverlapPair> PairsToRemove;
    for (const FOverlapPair& Pair : PreviousOverlappingPairs)
    {
        if (Pair.CompA == Component || Pair.CompB == Component)
        {
            PairsToRemove.Add(Pair);
        }
    }

    for (const FOverlapPair& PairToRemove : PairsToRemove)
    {
        PreviousOverlappingPairs.Remove(PairToRemove);
    }
}

namespace CollisionChecks
{
    bool CheckSphereSphereOverlap(const USphereComponent* SphereA, const USphereComponent* SphereB)
    {
        FVector PosA = SphereA->GetWorldLocation();
        float RadiusA = SphereA->GetScaledSphereRadius();
        FVector PosB = SphereB->GetWorldLocation();
        float RadiusB = SphereB->GetScaledSphereRadius();
        float DistSq = FVector::DistanceSquared(PosA, PosB);
        float RadiiSum = RadiusA + RadiusB;

        return DistSq <= (RadiiSum * RadiiSum);
    }

    bool CheckBoxBoxOverlap(const UBoxComponent* BoxA, const UBoxComponent* BoxB)
    {
        if (!BoxA || !BoxB) return false; // Null check

        FVector ExtentA = BoxA->GetScaledBoxExtent();
        FVector CenterA = BoxA->GetWorldLocation();
        FMatrix RotMatA = BoxA->GetRotationMatrix();

        FVector ExtentB = BoxB->GetScaledBoxExtent();
        FVector CenterB = BoxB->GetWorldLocation();
        FMatrix RotMatB = BoxB->GetRotationMatrix();
        // --- 분리 축 정리 (Separating Axis Theorem) ---
    
        TArray<FVector> AxesToTest;
        AxesToTest.Reserve(15); // 최대 15개 축

        const FVector AxisA_X = FMatrix::GetColumn(RotMatA, 0);
        const FVector AxisA_Y = FMatrix::GetColumn(RotMatA, 1);
        const FVector AxisA_Z = FMatrix::GetColumn(RotMatA, 2);

        const FVector AxisB_X = FMatrix::GetColumn(RotMatB, 0);
        const FVector AxisB_Y = FMatrix::GetColumn(RotMatB, 1);
        const FVector AxisB_Z = FMatrix::GetColumn(RotMatB, 2);

        // 1. Box A's 3 face normals (world space axes)
        AxesToTest.Add(AxisA_X);
        AxesToTest.Add(AxisA_Y);
        AxesToTest.Add(AxisA_Z);

        // 2. Box B's 3 face normals (world space axes)
        AxesToTest.Add(AxisB_X);
        AxesToTest.Add(AxisB_Y);
        AxesToTest.Add(AxisB_Z);

        // 3. Cross products of edges (9 axes)
        // Using the already extracted axes vectors
        const FVector AxesA[] = { AxisA_X, AxisA_Y, AxisA_Z };
        const FVector AxesB[] = { AxisB_X, AxisB_Y, AxisB_Z };

        // 3. 두 박스 모서리 방향 간의 외적 9개
        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                FVector CrossProduct = FVector::CrossProduct(AxesToTest[i], AxesToTest[j]);
                // 외적이 0벡터에 가까우면 (두 모서리가 평행하면) 유효한 축이 아님
                if (!CrossProduct.IsNearlyZero())
                {
                    AxesToTest.Add(CrossProduct.GetSafeNormal()); // 정규화하여 축으로 사용
                }
            }
        }
    
        // 각 축에 대해 투영하여 분리되는지 검사
        for (const FVector& Axis : AxesToTest)
        {
            // 축이 유효한지 확인 (Zero Vector 방지)
            if (Axis.IsNearlyZero()) continue;
    
            float MinA, MaxA, MinB, MaxB;
            ProjectBoxOntoAxis(Axis, CenterA, ExtentA, RotMatA, MinA, MaxA);
            ProjectBoxOntoAxis(Axis, CenterB, ExtentB, RotMatB, MinB, MaxB);
    
            // 한 축이라도 분리되면 오버랩하지 않음
            if (!OverlapOnAxis(MinA, MaxA, MinB, MaxB))
            {
                return false;
            }
        }
    
        // 모든 축에서 겹치면 오버랩
        return true;
    }
    bool CheckSphereBoxOverlap(const USphereComponent* Sphere, const UBoxComponent* Box)
    {
        if (!Sphere || !Box)
        {
            return false;
        }

        const FVector SphereCenter = Sphere->GetWorldLocation();
        const float SphereRadius = Sphere->GetScaledSphereRadius();
        const float SphereRadiusSq = SphereRadius * SphereRadius;

        const FVector BoxCenter = Box->GetWorldLocation();
        const FVector BoxExtent = Box->GetScaledBoxExtent();
        const FMatrix BoxRotMat = Box->GetRotationMatrix(); 

        const FVector SphereCenterRelativeToBox = SphereCenter - BoxCenter;
        const FMatrix InverseBoxRotMat = FMatrix::Transpose(BoxRotMat); // 회전 행렬의 역행렬은 전치 행렬
        const FVector SphereCenterLocal = FMatrix::TransformVector(SphereCenterRelativeToBox, InverseBoxRotMat);

        FVector ClosestPointLocal;
        ClosestPointLocal.X = FMath::Clamp(SphereCenterLocal.X, -BoxExtent.X, BoxExtent.X);
        ClosestPointLocal.Y = FMath::Clamp(SphereCenterLocal.Y, -BoxExtent.Y, BoxExtent.Y);
        ClosestPointLocal.Z = FMath::Clamp(SphereCenterLocal.Z, -BoxExtent.Z, BoxExtent.Z);

        const float DistSqLocal = FVector::DistanceSquared(SphereCenterLocal, ClosestPointLocal);
        return DistSqLocal <= SphereRadiusSq;
    }
}
