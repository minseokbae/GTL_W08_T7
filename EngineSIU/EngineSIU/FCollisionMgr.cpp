#include "FCollisionMgr.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/ShapeComponent.h"
#include "UObject/Casts.h"

//#include "CollisionHelpers.h"

namespace CollisionChecks
{
    bool CheckSphereSphereOverlap(const USphereComponent* SphereA, const USphereComponent* SphereB);
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

    //bool CheckCapsuleCapsuleOverlap(const UCapsuleComponent* CapsuleA, const UCapsuleComponent* CapsuleB)
    //{
    //    FVector PosA = CapsuleA->GetWorldLocation();
    //    return false; // 임시 반환
    //}
   
    //bool CheckBoxBoxOverlap(const UBoxComponent* BoxA, const UBoxComponent* BoxB)
    //{
    //    if (!BoxA || !BoxB) return false; // Null check
    //
    //    FTransform TransformA = BoxA->GetComponentTransform(); // GetComponentTransform이 월드 트랜스폼 제공
    //    FVector ExtentA = BoxA->GetScaledBoxExtent();
    //    FVector CenterA = TransformA.GetLocation();
    //    FRotationMatrix RotMatA(TransformA.GetRotation()); // 회전 행렬 사용
    //
    //    FTransform TransformB = BoxB->GetComponentTransform();
    //    FVector ExtentB = BoxB->GetScaledBoxExtent();
    //    FVector CenterB = TransformB.GetLocation();
    //    FRotationMatrix RotMatB(TransformB.GetRotation());
    //
    //    // --- 분리 축 정리 (Separating Axis Theorem) ---
    //
    //    // 테스트할 축들을 저장할 배열
    //    TArray<FVector> AxesToTest;
    //    AxesToTest.Reserve(15); // 최대 15개 축
    //
    //    // 1. Box A의 3개 면 법선 (월드 공간)
    //    AxesToTest.Add(RotMatA.GetScaledAxis(EAxis::X));
    //    AxesToTest.Add(RotMatA.GetScaledAxis(EAxis::Y));
    //    AxesToTest.Add(RotMatA.GetScaledAxis(EAxis::Z));
    //
    //    // 2. Box B의 3개 면 법선 (월드 공간)
    //    AxesToTest.Add(RotMatB.GetScaledAxis(EAxis::X));
    //    AxesToTest.Add(RotMatB.GetScaledAxis(EAxis::Y));
    //    AxesToTest.Add(RotMatB.GetScaledAxis(EAxis::Z));
    //
    //    // 3. 두 박스 모서리 방향 간의 외적 9개
    //    for (int i = 0; i < 3; ++i)
    //    {
    //        for (int j = 0; j < 3; ++j)
    //        {
    //            FVector CrossProduct = FVector::CrossProduct(AxesToTest[i], AxesToTest[3 + j]);
    //            // 외적이 0벡터에 가까우면 (두 모서리가 평행하면) 유효한 축이 아님
    //            if (!CrossProduct.IsNearlyZero())
    //            {
    //                AxesToTest.Add(CrossProduct.GetSafeNormal()); // 정규화하여 축으로 사용
    //            }
    //        }
    //    }
    //
    //    // 각 축에 대해 투영하여 분리되는지 검사
    //    for (const FVector& Axis : AxesToTest)
    //    {
    //        // 축이 유효한지 확인 (Zero Vector 방지)
    //        if (Axis.IsNearlyZero()) continue;
    //
    //        float MinA, MaxA, MinB, MaxB;
    //        ProjectBoxOntoAxis(Axis, CenterA, ExtentA, RotMatA, MinA, MaxA);
    //        ProjectBoxOntoAxis(Axis, CenterB, ExtentB, RotMatB, MinB, MaxB);
    //
    //        // 한 축이라도 분리되면 오버랩하지 않음
    //        if (!OverlapOnAxis(MinA, MaxA, MinB, MaxB))
    //        {
    //            return false;
    //        }
    //    }
    //
    //    // 모든 축에서 겹치면 오버랩
    //    return true;
    //}
    
    //bool CheckCapsuleSphereOverlap(const UCapsuleComponent* Capsule, const USphereComponent* Sphere)
    //{
    //    // TODO: 실제 캡슐-스피어 오버랩 검사 로직 구현
    //    // 1. 캡슐의 트랜스폼, 반지름, 절반 높이와 스피어의 위치, 반지름을 얻습니다.
    //    // 2. 스피어 중심점에서 캡슐의 중심 선분까지의 최단 거리를 계산합니다.
    //    // 3. 이 최단 거리가 캡슐 반지름과 스피어 반지름의 합보다 작거나 같으면 오버랩입니다.
    //    return false; // 임시 반환
    //}
    
    //bool CheckCapsuleBoxOverlap(const UCapsuleComponent* Capsule, const UBoxComponent* Box)
    //{
    //    // TODO: 실제 캡슐-박스 오버랩 검사 로직 구현 (SAT 변형 또는 다른 기법 사용)
    //    // SAT를 사용할 경우 축 검사가 더 복잡해집니다. (캡슐 투영 처리 등)
    //    // 또는 박스를 확장( Minkowski Sum 반대 개념)하고 캡슐 선분과의 교차 검사 등을 사용할 수 있습니다.
    //    return false; // 임시 반환
    //}
    
    //bool CheckSphereBoxOverlap(const USphereComponent* Sphere, const UBoxComponent* Box)
    //{
    //    // TODO: 실제 스피어-박스 오버랩 검사 로직 구현
    //    // 1. 스피어의 위치, 반지름과 박스의 트랜스폼, 절반 크기를 얻습니다.
    //    // 2. 스피어 중심점을 박스의 로컬 좌표계로 변환합니다.
    //    // 3. 박스 내에서 스피어 중심점에 가장 가까운 점(Closest Point)을 찾습니다. (AABB의 Closest Point 찾기 로직 활용)
    //    // 4. 스피어 중심점과 이 가장 가까운 점 사이의 거리 제곱을 계산합니다.
    //    // 5. 이 거리 제곱이 스피어 반지름의 제곱보다 작거나 같으면 오버랩입니다.
    //    return false; // 임시 반환
    //}

}
