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

FQuat FQuat::operator^(float t) const
{
    float Angle = 0;
    FVector Normal;

    ToAxisAngle(Normal, Angle);

    float TAngle = Angle * t;
    return FQuat(Normal, TAngle);
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

void FQuat::ToAxisAngle(FVector OutAxis, float OutAngle) const
{
    if (W > 1.0f)
        Normalize();

    OutAngle = 2.0f * acosf(W);
    float sinTheta = sqrtf(1.0f - W * W);

    if (sinTheta < 0.001f)
    {
        OutAxis = FVector(0, 0, 1);
    }
    else
    {
        OutAxis.X = X / sinTheta;
        OutAxis.Y = Y / sinTheta;
        OutAxis.Z = Z / sinTheta;
    }
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

FQuat FQuat::Slerp(const FQuat& A, const FQuat& B, float Alpha)
{
    FQuat NormalizedA = A.Normalize();
    FQuat NormalizedB = B.Normalize();

    float dot = NormalizedA.W * NormalizedB.W + NormalizedA.X * NormalizedB.X + NormalizedA.Y * NormalizedB.Y + NormalizedA.Z * NormalizedB.Z;

    if (dot < 0.0f)
    {
        NormalizedB = FQuat(-NormalizedB.W, -NormalizedB.X, -NormalizedB.Y, -NormalizedB.Z);
        dot = -dot;
    }

    const float DOT_THRESHOLD = 0.00095f;
    if (dot > DOT_THRESHOLD)
    {
        FQuat result = FQuat(
            FMath::Lerp(NormalizedA.W, NormalizedB.W, Alpha),
            FMath::Lerp(NormalizedA.X, NormalizedB.X, Alpha),
            FMath::Lerp(NormalizedA.Y, NormalizedB.Y, Alpha),
            FMath::Lerp(NormalizedA.Z, NormalizedB.Z, Alpha)
        );
        return result.Normalize();
    }

    float theta_0 = acosf(dot);
    float theta = theta_0 * Alpha;
    float sin_theta = sinf(theta);
    float sin_theta_0 = sinf(theta_0);

    float s0 = cosf(theta) - dot * sin_theta / sin_theta_0;
    float s1 = sin_theta / sin_theta_0;

    FQuat result = FQuat(
        s0 * NormalizedA.W + s1 * NormalizedB.W,
        s0 * NormalizedA.X + s1 * NormalizedB.X,
        s0 * NormalizedA.Y + s1 * NormalizedB.Y,
        s0 * NormalizedA.Z + s1 * NormalizedB.Z
    );

    return result.Normalize();
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
