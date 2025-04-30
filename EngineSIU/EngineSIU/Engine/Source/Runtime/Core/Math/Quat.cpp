#include "Quat.h"

#include "Vector.h"
#include "Matrix.h"
#include "Rotator.h"

// 허용 오차 값 정의 (필요시 조정)
#ifndef THRESH_QUAT_DOT_AS_ONE
#define THRESH_QUAT_DOT_AS_ONE (0.999999f)
#endif
#ifndef SMALL_NUMBER // 작은 수 정의 (엔진 공통 정의 사용 권장)
#define SMALL_NUMBER (1.e-8f)
#endif

const FQuat GIdentityQuat = FQuat(1.f, 0.f, 0.f, 0.f); 

FQuat::FQuat(const FVector& Axis, float Angle)
{
    float HalfAngle = Angle * 0.5f;
    float SinHalfAngle = sinf(HalfAngle);
    float CosHalfAngle = cosf(HalfAngle);

    X = Axis.X * SinHalfAngle;
    Y = Axis.Y * SinHalfAngle;
    Z = Axis.Z * SinHalfAngle;
    W = CosHalfAngle;
}

FQuat::FQuat(const FMatrix& InMatrix)
{
    float S;
    // Check diagonal (trace)
    const float trace = InMatrix.M[0][0] + InMatrix.M[1][1] + InMatrix.M[2][2]; // 행렬의 Trace 값 (대각합)

    if (trace > 0.0f) 
    {
        float InvS = FMath::InvSqrt(trace + 1.f);
        this->W = 0.5f * (1.f / InvS);
        S = 0.5f * InvS;

        this->X = ((InMatrix.M[1][2] - InMatrix.M[2][1]) * S);
        this->Y = ((InMatrix.M[2][0] - InMatrix.M[0][2]) * S);
        this->Z = ((InMatrix.M[0][1] - InMatrix.M[1][0]) * S);
    } 
    else 
    {
        // diagonal is negative
        int32 i = 0;

        if (InMatrix.M[1][1] > InMatrix.M[0][0])
            i = 1;

        if (InMatrix.M[2][2] > InMatrix.M[i][i])
            i = 2;

        static constexpr int32 nxt[3] = { 1, 2, 0 };
        const int32 j = nxt[i];
        const int32 k = nxt[j];
 
        S = InMatrix.M[i][i] - InMatrix.M[j][j] - InMatrix.M[k][k] + 1.0f;

        float InvS = FMath::InvSqrt(S);

        float qt[4];
        qt[i] = 0.5f * (1.f / InvS);

        S = 0.5f * InvS;

        qt[3] = (InMatrix.M[j][k] - InMatrix.M[k][j]) * S;
        qt[j] = (InMatrix.M[i][j] + InMatrix.M[j][i]) * S;
        qt[k] = (InMatrix.M[i][k] + InMatrix.M[k][i]) * S;

        this->X = qt[0];
        this->Y = qt[1];
        this->Z = qt[2];
        this->W = qt[3];

    }
}

FQuat FQuat::operator*(const FQuat& Other) const
{
    return FQuat(
            W * Other.W - X * Other.X - Y * Other.Y - Z * Other.Z,
            W * Other.X + X * Other.W + Y * Other.Z - Z * Other.Y,
            W * Other.Y - X * Other.Z + Y * Other.W + Z * Other.X,
            W * Other.Z + X * Other.Y - Y * Other.X + Z * Other.W
        );
}

FVector FQuat::RotateVector(const FVector& Vec) const
{
    // 벡터를 쿼터니언으로 변환
    FQuat vecQuat(0.0f, Vec.X, Vec.Y, Vec.Z);
    // 회전 적용 (q * vec * q^-1)
    FQuat conjugate = FQuat(W, -X, -Y, -Z); // 쿼터니언의 켤레
    FQuat result = *this * vecQuat * conjugate;

    return FVector(result.X, result.Y, result.Z); // 회전된 벡터 반환
}

bool FQuat::IsNormalized() const
{
    return fabs(W * W + X * X + Y * Y + Z * Z - 1.0f) < 1e-6f;
}

FQuat FQuat::Normalize() const
{
    float magnitude = sqrtf(W * W + X * X + Y * Y + Z * Z);
    return FQuat(W / magnitude, X / magnitude, Y / magnitude, Z / magnitude);
}

FQuat FQuat::FromAxisAngle(const FVector& Axis, float Angle)
{
    float halfAngle = Angle * 0.5f;
    float sinHalfAngle = sinf(halfAngle);
    return FQuat(cosf(halfAngle), Axis.X * sinHalfAngle, Axis.Y * sinHalfAngle, Axis.Z * sinHalfAngle);
}

FQuat FQuat::CreateRotation(float roll, float pitch, float yaw)
{
    // 각도를 라디안으로 변환
    float radRoll = roll * (PI / 180.0f);
    float radPitch = pitch * (PI / 180.0f);
    float radYaw = yaw * (PI / 180.0f);

    // 각 축에 대한 회전 쿼터니언 계산
    FQuat qRoll = FQuat(FVector(1.0f, 0.0f, 0.0f), radRoll); // X축 회전
    FQuat qPitch = FQuat(FVector(0.0f, 1.0f, 0.0f), radPitch); // Y축 회전
    FQuat qYaw = FQuat(FVector(0.0f, 0.0f, 1.0f), radYaw); // Z축 회전

    // 회전 순서대로 쿼터니언 결합 (Y -> X -> Z)
    return qRoll * qPitch * qYaw;
}

FMatrix FQuat::ToMatrix() const
{
    FMatrix RotationMatrix;
    RotationMatrix.M[0][0] = 1.0f - 2.0f * (Y * Y + Z * Z);
    RotationMatrix.M[0][1] = 2.0f * (X * Y - W * Z);
    RotationMatrix.M[0][2] = 2.0f * (X * Z + W * Y);
    RotationMatrix.M[0][3] = 0.0f;


    RotationMatrix.M[1][0] = 2.0f * (X * Y + W * Z);
    RotationMatrix.M[1][1] = 1.0f - 2.0f * (X * X + Z * Z);
    RotationMatrix.M[1][2] = 2.0f * (Y * Z - W * X);
    RotationMatrix.M[1][3] = 0.0f;

    RotationMatrix.M[2][0] = 2.0f * (X * Z - W * Y);
    RotationMatrix.M[2][1] = 2.0f * (Y * Z + W * X);
    RotationMatrix.M[2][2] = 1.0f - 2.0f * (X * X + Y * Y);
    RotationMatrix.M[2][3] = 0.0f;

    RotationMatrix.M[3][0] = RotationMatrix.M[3][1] = RotationMatrix.M[3][2] = 0.0f;
    RotationMatrix.M[3][3] = 1.0f;

    return RotationMatrix;
}

const FQuat& FQuat::Identity()
{
    return GIdentityQuat;
}

FQuat FQuat::Conjugate() const
{
    return FQuat(W, -X, -Y, -Z); // W는 그대로, X,Y,Z 부호 반전
}

FQuat FQuat::Inverse() const
{
    // 길이 제곱 계산
    float NormSq = W * W + X * X + Y * Y + Z * Z;
    // 0으로 나누는 것 방지
    if (NormSq > std::numeric_limits<float>::epsilon()) // 또는 적절한 작은 값 (e.g., SMALL_NUMBER)
    {
        float InvNormSq = 1.0f / NormSq;
        // 역 = 켤레 / 길이제곱
        return FQuat(W * InvNormSq, -X * InvNormSq, -Y * InvNormSq, -Z * InvNormSq);
    }
    else
    {
        // 길이가 0에 가까우면 단위 쿼터니언 반환 (오류 상황)
        return FQuat::Identity();
    }
}

// Slerp_NotNormalized가 실제 핵심 로직을 수행하도록 구현
FQuat FQuat::Slerp_NotNormalized(const FQuat& Quat1, const FQuat& Quat2, float SlerpAlpha)
{
    // 두 쿼터니언 간의 각도의 코사인 값 (내적)
    float CosTheta = FQuat::Dot(Quat1, Quat2);

    // 만약 내적이 음수이면, 쿼터니언 중 하나를 반전시켜 최단 경로를 따라 보간하도록 함
    // (q 와 -q 는 같은 회전을 나타냄)
    FQuat Quat2Corrected = Quat2;
    if (CosTheta < 0.0f)
    {
        CosTheta = -CosTheta;
        Quat2Corrected = -Quat2; // 부호 반전된 쿼터니언 사용
    }

    // 코사인 값이 1에 매우 가까우면 (각도가 0에 가까움), 선형 보간(Lerp) 사용 (수치 안정성 및 성능)
    if (CosTheta > THRESH_QUAT_DOT_AS_ONE)
    {
        // Lerp: (1 - t) * q1 + t * q2
        FQuat Result = Quat1 * (1.0f - SlerpAlpha) + Quat2Corrected * SlerpAlpha;
        // Lerp는 단위 길이를 보장하지 않으므로 정규화 필요
        return Result.Normalize(); // Normalize 함수가 원본을 바꾸지 않고 새 것을 반환한다고 가정
        // 만약 Normalize가 원본을 바꾼다면: Result.Normalize(); return Result;
    }
    else
    {
        // 표준 Slerp 공식 사용
        // 각도(theta) 계산
        float Theta = FMath::Acos(CosTheta); // acos 사용
        if (Theta < SMALL_NUMBER) // 각도가 0에 매우 가까운 예외 케이스 추가 처리
        {
            // Lerp와 동일하게 처리
            FQuat Result = Quat1 * (1.0f - SlerpAlpha) + Quat2Corrected * SlerpAlpha;
            return Result.Normalize();
        }

        float SinTheta = FMath::Sin(Theta); // sin 사용

        // SinTheta가 0에 가까운 경우 처리 (거의 180도 차이날 때 - CosTheta가 -1 근처)
        // 이 경우는 CosTheta < 0 체크 및 반전으로 인해 발생하기 어려움.
        // 하지만 방어적으로 체크 가능
        if (FMath::Abs(SinTheta) < SMALL_NUMBER)
        {
            // Lerp와 유사하게 처리하거나, 단순히 Quat1 또는 Quat2 반환 가능
            FQuat Result = Quat1 * (1.0f - SlerpAlpha) + Quat2Corrected * SlerpAlpha;
            return Result.Normalize();
        }

        // Slerp 계수 계산
        float Scale0 = FMath::Sin((1.0f - SlerpAlpha) * Theta) / SinTheta;
        float Scale1 = FMath::Sin(SlerpAlpha * Theta) / SinTheta;

        // 최종 보간된 쿼터니언 계산
        return Quat1 * Scale0 + Quat2Corrected * Scale1;
        // Slerp는 이론적으로 단위 길이를 유지하지만, 부동소수점 오류 누적 방지 위해
        // 마지막에 Normalize()를 한번 더 호출하는 경우도 있음 (선택 사항).
    }
}

// Slerp 함수는 Slerp_NotNormalized를 호출하여 구현
FQuat FQuat::Slerp(const FQuat& Quat1, const FQuat& Quat2, float SlerpAlpha)
{
    // Slerp는 입력이 단위 쿼터니언이라고 가정하고 최단 경로 보간을 수행
    return Slerp_NotNormalized(Quat1, Quat2, SlerpAlpha);
}

float FQuat::Dot(const FQuat& Q1, const FQuat& Q2)
{
    return Q1.W * Q2.W + Q1.X * Q2.X + Q1.Y * Q2.Y + Q1.Z * Q2.Z;
}

bool FQuat::Equals(const FQuat& Other, float Tolerance) const
{
    // 각 컴포넌트가 허용 오차 내에 있는지 확인
    return (FMath::Abs(W - Other.W) <= Tolerance &&
        FMath::Abs(X - Other.X) <= Tolerance &&
        FMath::Abs(Y - Other.Y) <= Tolerance &&
        FMath::Abs(Z - Other.Z) <= Tolerance) ||
        // 또는 방향이 반대여도 같은 회전을 나타내는 경우 (-q 와 q)
        (FMath::Abs(W + Other.W) <= Tolerance &&
            FMath::Abs(X + Other.X) <= Tolerance &&
            FMath::Abs(Y + Other.Y) <= Tolerance &&
            FMath::Abs(Z + Other.Z) <= Tolerance);
}

bool FQuat::operator==(const FQuat& Other) const
{
    return Equals(Other); // 기본 Tolerance 사용
}

bool FQuat::operator!=(const FQuat& Other) const
{
    return !Equals(Other); // 기본 Tolerance 사용
}

FQuat FQuat::operator+(const FQuat& Q) const
{
    return FQuat(W + Q.W, X + Q.X, Y + Q.Y, Z + Q.Z);
}

FQuat FQuat::operator*(float Scale) const
{
    return FQuat(W * Scale, X * Scale, Y * Scale, Z * Scale);
}

FQuat FQuat::operator/(float Scale) const
{
    const float InvScale = 1.0f / Scale;
    return FQuat(W * InvScale, X * InvScale, Y * InvScale, Z * InvScale);
}

FQuat FQuat::operator-() const
{
    return FQuat(-W, -X, -Y, -Z);
}

FRotator FQuat::ToRotator() const
{
    const float X_ = this->X;
    const float Y_ = this->Y;
    const float Z_ = this->Z;
    const float W_ = this->W;

    const float X2 = X_ * X_;
    const float Y2 = Y_ * Y_;
    const float Z2 = Z_ * Z_;
    const float W2 = W_ * W_;

    const float SingularityTest = W_ * Y_ - X_ * Z_;

    const float SingularityThreshold = 0.99999f; // Use a value appropriate for your float precision

    float PitchRad, YawRad, RollRad;

    if (SingularityTest > SingularityThreshold) // Singularity at north pole (Pitch = +90 degrees)
    {
        PitchRad = PI / 2.0f;
        YawRad = FMath::Atan2(2.f * (X_ * Y_ + W_ * Z_), W2 + X2 - Y2 - Z2);
        // YawRad = 2.f * FMath::Atan2(X_, W_); // Alternative simpler form often used
        RollRad = 0.0f;
    }
    else if (SingularityTest < -SingularityThreshold) // Singularity at south pole (Pitch = -90 degrees)
    {
        PitchRad = -PI / 2.0f; // -90 degrees in radians
        // Similar to the north pole, set Roll to 0 and solve for Yaw.
        YawRad = FMath::Atan2(2.f * (X_ * Y_ + W_ * Z_), W2 + X2 - Y2 - Z2);
        //YawRad = -2.f * FMath::Atan2(X_, W_); // Alternative simpler form often used
        RollRad = 0.0f;
    }
    else // Normal case (no singularity)
    {
        // Pitch (Y-rotation) from the singularity test term
        PitchRad = FMath::Asin(2.0f * SingularityTest); // asin range is [-PI/2, PI/2]

        // Yaw (Z-rotation)
        YawRad = FMath::Atan2(2.0f * (W_ * Z_ + X_ * Y_), 1.0f - 2.0f * (Y2 + Z2));
        // Alternative using squares: FMath::Atan2(2.0f * (W_ * Z_ + X_ * Y_), W2 + X2 - Y2 - Z2);

        // Roll (X-rotation)
        RollRad = FMath::Atan2(2.0f * (W_ * X_ + Y_ * Z_), 1.0f - 2.0f * (X2 + Y2));
        // Alternative using squares: FMath::Atan2(2.0f * (W_ * X_ + Y_ * Z_), W2 - X2 - Y2 + Z2);
    }

    // Convert radians to degrees for FRotator
    // Assumes FRotator stores Pitch, Yaw, Roll in degrees.
    // Uses FMath::RadiansToDegrees or manual conversion (angle * 180 / PI)
    const float PitchDeg = FMath::RadiansToDegrees(PitchRad);
    const float YawDeg = FMath::RadiansToDegrees(YawRad);
    const float RollDeg = FMath::RadiansToDegrees(RollRad);

    return FRotator(PitchDeg, YawDeg, RollDeg);
}
