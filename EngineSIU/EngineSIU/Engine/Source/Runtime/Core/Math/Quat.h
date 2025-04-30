#pragma once
#include "Serialization/Archive.h"

struct FVector;
struct FMatrix;
struct FRotator;

// 쿼터니언
struct FQuat
{
    float W, X, Y, Z;

    // 기본 생성자
    FQuat()
        : W(1.0f), X(0.0f), Y(0.0f), Z(0.0f)
    {}


    // FQuat 생성자 추가: 회전 축과 각도를 받아서 FQuat 생성
    FQuat(const FVector& Axis, float Angle);

    // W, X, Y, Z 값으로 초기화
    FQuat(float InW, float InX, float InY, float InZ)
        : W(InW), X(InX), Y(InY), Z(InZ)
    {}

    FQuat(const FMatrix& InMatrix);

    // 쿼터니언의 곱셈 연산 (회전 결합)
    FQuat operator*(const FQuat& Other) const;

    FQuat operator^(float t) const;

    // (쿼터니언) 벡터 회전
    FVector RotateVector(const FVector& Vec) const;

    // 단위 쿼터니언 여부 확인
    bool IsNormalized() const;

    // 쿼터니언 정규화 (단위 쿼터니언으로 만듬)
    FQuat Normalize() const;

    // 회전 각도와 축으로부터 쿼터니언 생성 (axis-angle 방식)
    static FQuat FromAxisAngle(const FVector& Axis, float Angle);

    void ToAxisAngle(FVector OutAxis, float OutAngle) const;

    static FQuat CreateRotation(float roll, float pitch, float yaw);

    //회전 용 lerp
    //static FQuat Slerp(const FQuat& A, const FQuat& B, float Alpha);

    // 쿼터니언을 회전 행렬로 변환
    FMatrix ToMatrix() const;

    // 쿼터니언의 역을 반환
    FQuat Inverse() const;

    // 쿼터니언의 켤레(conjugate)를 반환
    // 단위 쿼터니언의 경우 역과 같음
    FQuat Conjugate() const;

    /**
     * 두 단위 쿼터니언 사이를 구면 선형 보간합니다.
     * @param Quat1 시작 쿼터니언 (단위 쿼터니언이어야 함)
     * @param Quat2 끝 쿼터니언 (단위 쿼터니언이어야 함)
     * @param Slerp 알파 값 (0.0 ~ 1.0)
     * @return 보간된 쿼터니언
     */
    static FQuat Slerp(const FQuat& Quat1, const FQuat& Quat2, float Slerp);

    /**
     * 두 단위 쿼터니언 사이를 최단 경로로 구면 선형 보간합니다.
     * Slerp와 유사하지만, 180도 이상 회전하지 않도록 합니다.
     * @param Quat1 시작 쿼터니언 (단위 쿼터니언이어야 함)
     * @param Quat2 끝 쿼터니언 (단위 쿼터니언이어야 함)
     * @param Slerp 알파 값 (0.0 ~ 1.0)
     * @return 보간된 쿼터니언
     */
    static FQuat Slerp_NotNormalized(const FQuat& Quat1, const FQuat& Quat2, float Slerp); // UE 스타일 이름

    /** 단위 쿼터니언 (회전 없음)을 반환합니다. */
    static const FQuat& Identity();
    static float Dot(const FQuat& Q1, const FQuat& Q2);

    bool Equals(const FQuat& Other, float Tolerance = 1.e-4f) const;

    /** 연산자 오버로딩 (Equals 호출) */
    bool operator==(const FQuat& Other) const;
    bool operator!=(const FQuat& Other) const;

    FQuat operator+(const FQuat& Q) const;
    FQuat operator*(float Scale) const;
    FQuat operator/(float Scale) const;
    FQuat operator-() const;


    FRotator ToRotator() const;

};

inline FArchive& operator<<(FArchive& Ar, FQuat& Q)
{
    return Ar << Q.X << Q.Y << Q.Z << Q.W;
}
