#pragma once
#include "Math/Vector.h"
#include "Math/MathUtility.h"
#include "Math/Matrix.h"

//// --- Helper Function for SAT ---
//// �� ���� [minA, maxA] �� [minB, maxB] �� ��ġ���� Ȯ��
//inline bool OverlapOnAxis(float ProjA_Min, float ProjA_Max, float ProjB_Min, float ProjB_Max)
//{
//    return ProjA_Max >= ProjB_Min && ProjB_Max >= ProjA_Min;
//}
//
//// �ڽ��� �־��� �࿡ �����Ͽ� �ּ�/�ִ밪�� ���
//inline void ProjectBoxOntoAxis(const FVector& Axis, const FVector& BoxCenter, const FVector& BoxExtent, const FMatrix& BoxRotMat, float& OutMin, float& OutMax)
//{
//    // �ڽ��� ���� �� (���� ���� ����)
//    FVector AxisX = BoxRotMat.GetScaledAxis(EAxis::X);
//    FVector AxisY = BoxRotMat.GetScaledAxis(EAxis::Y);
//    FVector AxisZ = BoxRotMat.GetScaledAxis(EAxis::Z);
//
//
//    // �ڽ� �߽��� �࿡ ����
//    float CenterProj = FVector::DotProduct(BoxCenter, Axis);
//
//    // �ڽ� Extent�� �� �� �������� ������ ���� ���밪�� ���� (������ ������ ���)
//    float ProjectedRadius =
//        FMath::Abs(FVector::DotProduct(AxisX * BoxExtent.X, Axis)) +
//        FMath::Abs(FVector::DotProduct(AxisY * BoxExtent.Y, Axis)) +
//        FMath::Abs(FVector::DotProduct(AxisZ * BoxExtent.Z, Axis));
//
//    OutMin = CenterProj - ProjectedRadius;
//    OutMax = CenterProj + ProjectedRadius;
//}
//
//// ���� ���� ������ �־��� �� P���� ���� ����� ���� ã��
//FVector ClosestPointOnSegment(const FVector& SegmentStart, const FVector& SegmentEnd, const FVector& Point)
//{
//    FVector SegmentDir = SegmentEnd - SegmentStart;
//    float SegmentLenSq = SegmentDir.SizeSquared();
//
//    // ������ ���̰� 0�� ������ �������� ��ȯ
//    if (SegmentLenSq < KINDA_SMALL_NUMBER)
//    {
//        return SegmentStart;
//    }
//
//    // Point�� ���� ���� ���Ϳ� �����Ͽ� �Ķ���� t ���
//    // t = Dot(Point - Start, End - Start) / Dot(End - Start, End - Start)
//    float t = FVector::DotProduct(Point - SegmentStart, SegmentDir) / SegmentLenSq;
//
//    // t�� [0, 1] ������ ���� (Clamp)
//    t = FMath::Clamp(t, 0.0f, 1.0f);
//
//    // ���� ���� ���� ����� �� ���
//    return SegmentStart + SegmentDir * t;
//}
//
//// OBB(Oriented Bounding Box) ����/ǥ�鿡�� �־��� �� Point�� ���� ����� ���� ã��
//FVector ClosestPointOnOBB(const FVector& Point, const FMatrix& BoxTransform, const FVector& BoxExtent)
//{
//    //FVector BoxCenter = FVector::;
//    //FQuat BoxRotation = BoxTransform.GetRotation();
//
//    // Point�� �ڽ��� ���� ��ǥ��� ��ȯ
//    FVector PointLocal = BoxRotation.UnrotateVector(Point - BoxCenter);
//
//    // ���� ��ǥ�迡�� AABB�� ���� ����� �� ã�� (Clamp)
//    FVector ClosestPointLocal;
//    ClosestPointLocal.X = FMath::Clamp(PointLocal.X, -BoxExtent.X, BoxExtent.X);
//    ClosestPointLocal.Y = FMath::Clamp(PointLocal.Y, -BoxExtent.Y, BoxExtent.Y);
//    ClosestPointLocal.Z = FMath::Clamp(PointLocal.Z, -BoxExtent.Z, BoxExtent.Z);
//
//    // ���� ��ǥ���� ���� ����� ���� �ٽ� ���� ��ǥ��� ��ȯ
//    FVector ClosestPointWorld = BoxCenter + BoxRotation.RotateVector(ClosestPointLocal);
//
//    return ClosestPointWorld;
//}
//
//// Capsule-Sphere ������ �˻�
//bool CheckCapsuleSphereOverlap(const UCapsuleComponent* Capsule, const USphereComponent* Sphere)
//{
//    if (!Capsule || !Sphere) return false; // Null check
//
//    FTransform CapsuleTransform = Capsule->GetComponentTransform();
//    float CapsuleRadius = Capsule->GetScaledCapsuleRadius();
//    float CapsuleHalfHeight = Capsule->GetScaledCapsuleHalfHeight();
//    FVector CapsuleUp = CapsuleTransform.GetUnitAxis(EAxis::Z); // ĸ���� ���� Z�� ����
//    FVector CapsuleCenter = CapsuleTransform.GetLocation();
//
//    // ĸ�� �߽� ������ �� ���� ���
//    FVector CapsuleSegmentStart = CapsuleCenter - CapsuleUp * (CapsuleHalfHeight - CapsuleRadius); // �ݱ� �߽� ���
//    FVector CapsuleSegmentEnd = CapsuleCenter + CapsuleUp * (CapsuleHalfHeight - CapsuleRadius);
//
//    FVector SphereCenter = Sphere->GetWorldLocation();
//    float SphereRadius = Sphere->GetScaledSphereRadius();
//
//    // ���Ǿ� �߽������� ĸ�� �߽� ���б��� ���� ����� �� ã��
//    FVector ClosestPoint = ClosestPointOnSegment(CapsuleSegmentStart, CapsuleSegmentEnd, SphereCenter);
//
//    // ���� ����� ���� ���Ǿ� �߽��� ������ �Ÿ� ���� ���
//    float DistSq = FVector::DistSquared(ClosestPoint, SphereCenter);
//    float RadiiSum = CapsuleRadius + SphereRadius;
//
//    // �Ÿ� ���� <= ������ �� ���� �̸� ������
//    return DistSq <= (RadiiSum * RadiiSum);
//}
//
//// Sphere-Box ������ �˻�
//bool CheckSphereBoxOverlap(const USphereComponent* Sphere, const UBoxComponent* Box)
//{
//    if (!Sphere || !Box) return false; // Null check
//
//    FVector SphereCenter = Sphere->GetWorldLocation();
//    float SphereRadius = Sphere->GetScaledSphereRadius();
//    float SphereRadiusSq = SphereRadius * SphereRadius; // �̸� ���� ���
//
//    FTransform BoxTransform = Box->GetComponentTransform();
//    FVector BoxExtent = Box->GetScaledBoxExtent();
//
//    // �ڽ� ������ ���Ǿ� �߽����� ���� ����� �� ã��
//    FVector ClosestPoint = ClosestPointOnOBB(SphereCenter, BoxTransform, BoxExtent);
//
//    // ���Ǿ� �߽����� ���� ����� �� ������ �Ÿ� ���� ���
//    float DistSq = FVector::DistSquared(SphereCenter, ClosestPoint);
//
//    // �Ÿ� ���� <= ���Ǿ� ������ ���� �̸� ������
//    return DistSq <= SphereRadiusSq;
//}
//
//// Capsule-Box ������ �˻� (���� ����� �� ���� ���)
//// ����: �� ����� �Ϻ����� ���� �� ������, �� ��Ȯ�� ���(��: SAT Ȯ��)�� �ξ� �����մϴ�.
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
//    // ĸ�� �߽� ������ �� ���� ��� (�ݱ� �߽� ��� ���� - ���� ��ü�� �ʿ�)
//    FVector CapsuleSegmentStart = CapsuleCenter - CapsuleUp * CapsuleHalfHeight;
//    FVector CapsuleSegmentEnd = CapsuleCenter + CapsuleUp * CapsuleHalfHeight;
//
//    FTransform BoxTransform = Box->GetComponentTransform();
//    FVector BoxExtent = Box->GetScaledBoxExtent();
//
//    // �ڽ� ������ ĸ�� ���п� ���� ����� ���� ã�ƾ� ��. �̴� ������ �����.
//    // ���⼭�� �ٻ����� ���ٹ�:
//    // 1. �ڽ� ������ ĸ�� ���� ���� '� ��'�� ���� ����� ���� ã�´�.
//    // 2. �� ���� ĸ�� ���� ������ ���� �Ÿ��� ����Ѵ�.
//    // 3. �� �Ÿ��� ĸ�� ���������� �۰ų� ������ ��ģ�ٰ� ����.
//
//    // ���� ����� ���� ã�� ���� �����ϹǷ�, ���⼭�� Unreal�� ���� �Լ� ����� �����ϰų�,
//    // FMath::SegmentDistToZeroLengthSegment() �� ClosestPointOnOBB() �� �����ؾ� �մϴ�.
//    // �Ʒ��� �������� �����̸�, �����δ� �� ������ Segment-OBB �Ÿ� �Լ��� �ʿ��մϴ�.
//
//    // ����: �ڽ� �߽ɿ��� ĸ�� ���б��� ���� ����� ���� ã�� (�ſ� �ܼ�ȭ�� ����)
//    FVector ClosestPointOnCapsuleSegment = ClosestPointOnSegment(CapsuleSegmentStart, CapsuleSegmentEnd, BoxTransform.GetLocation());
//
//    // �� ���� �������� �ڽ� ���� ���� ����� ���� ã��
//    FVector ClosestPointOnBox = ClosestPointOnOBB(ClosestPointOnCapsuleSegment, BoxTransform, BoxExtent);
//
//    // ���� ClosestPointOnBox�� ���� ĸ�� ���� ������ ���� ª�� �Ÿ��� ����ؾ� ��.
//    // �� �Ÿ��� CapsuleRadius���� ������ �浹.
//    FVector ClosestPointOnSegmentToBoxPoint = ClosestPointOnSegment(CapsuleSegmentStart, CapsuleSegmentEnd, ClosestPointOnBox);
//
//    float DistSq = FVector::DistSquared(ClosestPointOnBox, ClosestPointOnSegmentToBoxPoint);
//
//    return DistSq <= (CapsuleRadius * CapsuleRadius);
//
//    // ����: �� Capsule-Box ������ �ſ� �ܼ�ȭ�� �����Դϴ�. ��Ȯ�� �浹 �˻��
//    // FMath::SegmentDistToSegmentSquared �Ǵ� ���� Segment-OBB �˰����� �ʿ��մϴ�.
//    // �𸮾� ������ ���������� �� �����ϰ� ��Ȯ�� ����� ����մϴ�.
//}