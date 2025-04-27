#pragma once
#include "Math/Vector.h"
#include "Math/MathUtility.h"
#include "Math/Matrix.h"

//// --- Helper Function for SAT ---
//// 두 구간 [minA, maxA] 와 [minB, maxB] 가 겹치는지 확인
//inline bool OverlapOnAxis(float ProjA_Min, float ProjA_Max, float ProjB_Min, float ProjB_Max)
//{
//    return ProjA_Max >= ProjB_Min && ProjB_Max >= ProjA_Min;
//}
//
//// 박스를 주어진 축에 투영하여 최소/최대값을 계산
//inline void ProjectBoxOntoAxis(const FVector& Axis, const FVector& BoxCenter, const FVector& BoxExtent, const FMatrix& BoxRotMat, float& OutMin, float& OutMax)
//{
//    // 박스의 로컬 축 (월드 공간 기준)
//    FVector AxisX = BoxRotMat.GetScaledAxis(EAxis::X);
//    FVector AxisY = BoxRotMat.GetScaledAxis(EAxis::Y);
//    FVector AxisZ = BoxRotMat.GetScaledAxis(EAxis::Z);
//
//
//    // 박스 중심을 축에 투영
//    float CenterProj = FVector::DotProduct(BoxCenter, Axis);
//
//    // 박스 Extent를 각 축 방향으로 투영한 값의 절대값을 더함 (투영된 반지름 계산)
//    float ProjectedRadius =
//        FMath::Abs(FVector::DotProduct(AxisX * BoxExtent.X, Axis)) +
//        FMath::Abs(FVector::DotProduct(AxisY * BoxExtent.Y, Axis)) +
//        FMath::Abs(FVector::DotProduct(AxisZ * BoxExtent.Z, Axis));
//
//    OutMin = CenterProj - ProjectedRadius;
//    OutMax = CenterProj + ProjectedRadius;
//}
//
//// 선분 위의 점에서 주어진 점 P까지 가장 가까운 점을 찾음
//FVector ClosestPointOnSegment(const FVector& SegmentStart, const FVector& SegmentEnd, const FVector& Point)
//{
//    FVector SegmentDir = SegmentEnd - SegmentStart;
//    float SegmentLenSq = SegmentDir.SizeSquared();
//
//    // 선분의 길이가 0에 가까우면 시작점을 반환
//    if (SegmentLenSq < KINDA_SMALL_NUMBER)
//    {
//        return SegmentStart;
//    }
//
//    // Point를 선분 방향 벡터에 투영하여 파라미터 t 계산
//    // t = Dot(Point - Start, End - Start) / Dot(End - Start, End - Start)
//    float t = FVector::DotProduct(Point - SegmentStart, SegmentDir) / SegmentLenSq;
//
//    // t를 [0, 1] 범위로 제한 (Clamp)
//    t = FMath::Clamp(t, 0.0f, 1.0f);
//
//    // 선분 위의 가장 가까운 점 계산
//    return SegmentStart + SegmentDir * t;
//}
//
//// OBB(Oriented Bounding Box) 내부/표면에서 주어진 점 Point에 가장 가까운 점을 찾음
//FVector ClosestPointOnOBB(const FVector& Point, const FMatrix& BoxTransform, const FVector& BoxExtent)
//{
//    //FVector BoxCenter = FVector::;
//    //FQuat BoxRotation = BoxTransform.GetRotation();
//
//    // Point를 박스의 로컬 좌표계로 변환
//    FVector PointLocal = BoxRotation.UnrotateVector(Point - BoxCenter);
//
//    // 로컬 좌표계에서 AABB에 가장 가까운 점 찾기 (Clamp)
//    FVector ClosestPointLocal;
//    ClosestPointLocal.X = FMath::Clamp(PointLocal.X, -BoxExtent.X, BoxExtent.X);
//    ClosestPointLocal.Y = FMath::Clamp(PointLocal.Y, -BoxExtent.Y, BoxExtent.Y);
//    ClosestPointLocal.Z = FMath::Clamp(PointLocal.Z, -BoxExtent.Z, BoxExtent.Z);
//
//    // 로컬 좌표계의 가장 가까운 점을 다시 월드 좌표계로 변환
//    FVector ClosestPointWorld = BoxCenter + BoxRotation.RotateVector(ClosestPointLocal);
//
//    return ClosestPointWorld;
//}
//
//// Capsule-Sphere 오버랩 검사
//bool CheckCapsuleSphereOverlap(const UCapsuleComponent* Capsule, const USphereComponent* Sphere)
//{
//    if (!Capsule || !Sphere) return false; // Null check
//
//    FTransform CapsuleTransform = Capsule->GetComponentTransform();
//    float CapsuleRadius = Capsule->GetScaledCapsuleRadius();
//    float CapsuleHalfHeight = Capsule->GetScaledCapsuleHalfHeight();
//    FVector CapsuleUp = CapsuleTransform.GetUnitAxis(EAxis::Z); // 캡슐은 보통 Z축 기준
//    FVector CapsuleCenter = CapsuleTransform.GetLocation();
//
//    // 캡슐 중심 선분의 양 끝점 계산
//    FVector CapsuleSegmentStart = CapsuleCenter - CapsuleUp * (CapsuleHalfHeight - CapsuleRadius); // 반구 중심 고려
//    FVector CapsuleSegmentEnd = CapsuleCenter + CapsuleUp * (CapsuleHalfHeight - CapsuleRadius);
//
//    FVector SphereCenter = Sphere->GetWorldLocation();
//    float SphereRadius = Sphere->GetScaledSphereRadius();
//
//    // 스피어 중심점에서 캡슐 중심 선분까지 가장 가까운 점 찾기
//    FVector ClosestPoint = ClosestPointOnSegment(CapsuleSegmentStart, CapsuleSegmentEnd, SphereCenter);
//
//    // 가장 가까운 점과 스피어 중심점 사이의 거리 제곱 계산
//    float DistSq = FVector::DistSquared(ClosestPoint, SphereCenter);
//    float RadiiSum = CapsuleRadius + SphereRadius;
//
//    // 거리 제곱 <= 반지름 합 제곱 이면 오버랩
//    return DistSq <= (RadiiSum * RadiiSum);
//}
//
//// Sphere-Box 오버랩 검사
//bool CheckSphereBoxOverlap(const USphereComponent* Sphere, const UBoxComponent* Box)
//{
//    if (!Sphere || !Box) return false; // Null check
//
//    FVector SphereCenter = Sphere->GetWorldLocation();
//    float SphereRadius = Sphere->GetScaledSphereRadius();
//    float SphereRadiusSq = SphereRadius * SphereRadius; // 미리 제곱 계산
//
//    FTransform BoxTransform = Box->GetComponentTransform();
//    FVector BoxExtent = Box->GetScaledBoxExtent();
//
//    // 박스 위에서 스피어 중심점에 가장 가까운 점 찾기
//    FVector ClosestPoint = ClosestPointOnOBB(SphereCenter, BoxTransform, BoxExtent);
//
//    // 스피어 중심점과 가장 가까운 점 사이의 거리 제곱 계산
//    float DistSq = FVector::DistSquared(SphereCenter, ClosestPoint);
//
//    // 거리 제곱 <= 스피어 반지름 제곱 이면 오버랩
//    return DistSq <= SphereRadiusSq;
//}
//
//// Capsule-Box 오버랩 검사 (가장 가까운 점 접근 방식)
//// 참고: 이 방법은 완벽하지 않을 수 있으며, 더 정확한 방법(예: SAT 확장)은 훨씬 복잡합니다.
//bool CheckCapsuleBoxOverlap(const UCapsuleComponent* Capsule, const UBoxComponent* Box)
//{
//    if (!Capsule || !Box) return false; // Null check
//
//    FTransform CapsuleTransform = Capsule->GetComponentTransform();
//    float CapsuleRadius = Capsule->GetScaledCapsuleRadius();
//    float CapsuleHalfHeight = Capsule->GetScaledCapsuleHalfHeight();
//    FVector CapsuleUp = CapsuleTransform.GetUnitAxis(EAxis::Z);
//    FVector CapsuleCenter = CapsuleTransform.GetLocation();
//
//    // 캡슐 중심 선분의 양 끝점 계산 (반구 중심 고려 안함 - 선분 자체만 필요)
//    FVector CapsuleSegmentStart = CapsuleCenter - CapsuleUp * CapsuleHalfHeight;
//    FVector CapsuleSegmentEnd = CapsuleCenter + CapsuleUp * CapsuleHalfHeight;
//
//    FTransform BoxTransform = Box->GetComponentTransform();
//    FVector BoxExtent = Box->GetScaledBoxExtent();
//
//    // 박스 위에서 캡슐 선분에 가장 가까운 점을 찾아야 함. 이는 복잡한 계산임.
//    // 여기서는 근사적인 접근법:
//    // 1. 박스 위에서 캡슐 선분 위의 '어떤 점'에 가장 가까운 점을 찾는다.
//    // 2. 그 점과 캡슐 선분 사이의 실제 거리를 계산한다.
//    // 3. 그 거리가 캡슐 반지름보다 작거나 같으면 겹친다고 본다.
//
//    // 가장 가까운 점을 찾는 것은 복잡하므로, 여기서는 Unreal의 내장 함수 사용을 가정하거나,
//    // FMath::SegmentDistToZeroLengthSegment() 와 ClosestPointOnOBB() 를 조합해야 합니다.
//    // 아래는 개념적인 구현이며, 실제로는 더 정교한 Segment-OBB 거리 함수가 필요합니다.
//
//    // 예시: 박스 중심에서 캡슐 선분까지 가장 가까운 점을 찾음 (매우 단순화된 접근)
//    FVector ClosestPointOnCapsuleSegment = ClosestPointOnSegment(CapsuleSegmentStart, CapsuleSegmentEnd, BoxTransform.GetLocation());
//
//    // 이 점을 기준으로 박스 상의 가장 가까운 점을 찾음
//    FVector ClosestPointOnBox = ClosestPointOnOBB(ClosestPointOnCapsuleSegment, BoxTransform, BoxExtent);
//
//    // 이제 ClosestPointOnBox와 실제 캡슐 선분 사이의 가장 짧은 거리를 계산해야 함.
//    // 이 거리가 CapsuleRadius보다 작으면 충돌.
//    FVector ClosestPointOnSegmentToBoxPoint = ClosestPointOnSegment(CapsuleSegmentStart, CapsuleSegmentEnd, ClosestPointOnBox);
//
//    float DistSq = FVector::DistSquared(ClosestPointOnBox, ClosestPointOnSegmentToBoxPoint);
//
//    return DistSq <= (CapsuleRadius * CapsuleRadius);
//
//    // 참고: 위 Capsule-Box 로직은 매우 단순화된 버전입니다. 정확한 충돌 검사는
//    // FMath::SegmentDistToSegmentSquared 또는 전용 Segment-OBB 알고리즘이 필요합니다.
//    // 언리얼 엔진은 내부적으로 더 복잡하고 정확한 방법을 사용합니다.
//}