#include "CameraShakeBase.h"
#include "Math/Rotator.h"
#include "PlayerCameraManager.h"
#include "CameraComponent.h"

UCameraShakeBase::UCameraShakeBase()
    : ElapsedTime(0.f)
    , bIsFinished(false)
    , bIsActive(false)
    , InitialSeed(1337) // 기본 시드값 설정
{
    Priority = 0;
}
void UCameraShakeBase::PlayShake()
{
    InitializeNoiseGenerator();

    ElapsedTime = 0.f;
    bIsFinished = false;
    bIsActive = true;

    NoiseGenerator.SetSeed(InitialSeed);
}

void UCameraShakeBase::StopShake(bool bImmediate)
{
    if (bImmediate)
    {
        bIsActive = false;
        bIsFinished = true; 
        ElapsedTime = Duration;
    }
    else
    {
        if (Duration > 0.f && ElapsedTime < Duration - BlendOutTime)
        {
            ElapsedTime = Duration - BlendOutTime;
        }
        // bIsActive는 ModifyCamera 내부에서 BlendWeight가 0이 되거나 Duration이 끝나면 false로 설정될 것
    }
}

bool UCameraShakeBase::ModifyCamera(float DeltaTime, APlayerCameraManager* NewCameraManager)
{
    FVector CachedLocation = NewCameraManager->GetCachedCamera()->GetRelativeLocation();
    FRotator CachedRotation = NewCameraManager->GetCachedCamera()->GetRelativeRotation();

    if (!bIsActive || bIsFinished || IsDisabled())
    {
        return false;
    }

    ElapsedTime += DeltaTime;

    const float CurrentBlendWeight = GetBlendWeight();

    if (CurrentBlendWeight <= 0.0f && Duration > 0.0f)
    {
        if (ElapsedTime >= Duration)
        {
            bIsFinished = true;
            bIsActive = false;
            // 필요 시 DisableModifier() 호출 혹은 Manager에게 완료 알림
            return false;
        }
        // BlendOut 중 가중치가 0이 된 경우 아직 Finished는 아님 (Duration 끝까지 기다림)
        // 하지만 더 이상 계산할 필요 없으므로 return 가능
        if (ElapsedTime >= Duration - BlendOutTime + KINDA_SMALL_NUMBER) // BlendOut 구간이고 가중치가 0이면 끝난 것으로 간주 가능
        {
            // 혹은 return; 만 해서 끝날 때까지 유지
            // 여기서는 일단 return
            return false;
        }

    }

    // 현재 시간 기준 셰이크 오프셋 계산
    FVector RawLocationOffset = FVector::ZeroVector;
    FRotator RawRotationOffset = FRotator::ZeroRotator;
    CalculateShakeOffsets(ElapsedTime, RawLocationOffset, RawRotationOffset);

    const FVector LocalLocationOffset = RawLocationOffset * CurrentBlendWeight;
    const FRotator LocalRotationOffset = RawRotationOffset * CurrentBlendWeight;

    FQuat BaseRelativeQuat = CachedRotation.ToQuaternion();
    FVector OffsetInPawnSpace = BaseRelativeQuat.RotateVector(LocalLocationOffset);
    CachedLocation += OffsetInPawnSpace;

    FQuat ShakeQuat = LocalRotationOffset.ToQuaternion();
    FQuat ResultQuat = BaseRelativeQuat * ShakeQuat;
    CachedRotation = ResultQuat.ToRotator();

    NewCameraManager->GetCachedCamera()->SetRelativeLocation(CachedLocation);
    NewCameraManager->GetCachedCamera()->SetRelativeRotation(CachedRotation);
    return true;
}

//void UCameraShakeBase::ModifyCamera(float DeltaTime, FVector ViewLocation, FRotator ViewRotation, float FOV, FVector& NewViewLocation, FRotator& NewViewRotation, float& NewFOV)
//{
//    NewViewLocation = ViewLocation;
//    NewViewRotation = ViewRotation;
//    NewFOV = FOV;
//
//    //bIsActive 이게 true가 아님.
//    if (!bIsActive || bIsFinished || IsDisabled())
//    {
//         return;
//    }
//
//    ElapsedTime += DeltaTime;
//
//    const float CurrentBlendWeight = GetBlendWeight();
//
//    if (CurrentBlendWeight <= 0.0f && Duration > 0.0f) 
//    {
//        if (ElapsedTime >= Duration)
//        {
//            bIsFinished = true;
//            bIsActive = false;
//            // 필요 시 DisableModifier() 호출 혹은 Manager에게 완료 알림
//            return;
//        }
//        // BlendOut 중 가중치가 0이 된 경우 아직 Finished는 아님 (Duration 끝까지 기다림)
//        // 하지만 더 이상 계산할 필요 없으므로 return 가능
//        if (ElapsedTime >= Duration - BlendOutTime + KINDA_SMALL_NUMBER) // BlendOut 구간이고 가중치가 0이면 끝난 것으로 간주 가능
//        {
//            // 혹은 return; 만 해서 끝날 때까지 유지
//            // 여기서는 일단 return
//            return;
//        }
//
//    }
//
//    // 현재 시간 기준 셰이크 오프셋 계산
//    FVector RawLocationOffset = FVector::ZeroVector;
//    FRotator RawRotationOffset = FRotator::ZeroRotator; 
//    CalculateShakeOffsets(ElapsedTime, RawLocationOffset, RawRotationOffset);
//
//    const FVector LocalLocationOffset = RawLocationOffset * CurrentBlendWeight; 
//    const FRotator LocalRotationOffset = RawRotationOffset * CurrentBlendWeight;
//
//    FQuat BaseRelativeQuat = ViewRotation.ToQuaternion();
//    FVector OffsetInPawnSpace = BaseRelativeQuat.RotateVector(LocalLocationOffset);
//    NewViewLocation += OffsetInPawnSpace;
//
//    FQuat ShakeQuat = LocalRotationOffset.ToQuaternion();
//    FQuat ResultQuat = BaseRelativeQuat * ShakeQuat;
//    NewViewRotation = ResultQuat.ToRotator();
//}

float UCameraShakeBase::GetBlendWeight() const
{
    if (!bIsActive || bIsFinished)
    {
        return 0.0f;
    }

    float BlendWeight = 1.0f;

    if (Duration <= 0.0f)
    {
        if (BlendInTime > 0.0f)
        {
            BlendWeight = std::min(1.0f, ElapsedTime / BlendInTime);
        }
    }
    else
    {
        // Blend In
        if (BlendInTime > 0.0f && ElapsedTime < BlendInTime)
        {
            BlendWeight = ElapsedTime / BlendInTime;
        }

        // Blend Out은 Blend In보다 우선순위가 낮아야 겹칠 때 올바르게 처리됨
        if (BlendOutTime > 0.0f && ElapsedTime > Duration - BlendOutTime)
        {
            // BlendIn/Out이 겹치는 구간에서 작은 값을 선택하도록 min 사용
            float BlendOutWeight = std::max(0.0f, (Duration - ElapsedTime) / BlendOutTime);
            BlendWeight = std::min(BlendWeight, BlendOutWeight);
        }
    }
    return std::max(0.0f, std::min(BlendWeight, 1.0f));
}

void UCameraShakeBase::CalculateShakeOffsets(float CurrentTime, FVector& OutLocationOffset, FRotator& OutRotationOffset)
{
    //const float LocFreqMultiplier = ShakePattern.LocationFrequencyMultiplier;
    //const float LocAmplMultiplier = ShakePattern.LocationAmplitudeMultiplier;
    //const float LocTime = CurrentTime * LocFreqMultiplier;

    //float RawLocX = NoiseGenerator.GetNoise(LocTime, 10.0f) * ShakePattern.LocationAmplitude.X * LocAmplMultiplier;
    //float RawLocY = NoiseGenerator.GetNoise(LocTime, 20.0f) * ShakePattern.LocationAmplitude.Y * LocAmplMultiplier;
    //float RawLocZ = NoiseGenerator.GetNoise(LocTime, 30.0f) * ShakePattern.LocationAmplitude.Z * LocAmplMultiplier;

    //OutLocationOffset.X = FMath::Clamp(RawLocX, -ShakePattern.MaxLocationOffsetClamp.X, ShakePattern.MaxLocationOffsetClamp.X);
    //OutLocationOffset.Y = FMath::Clamp(RawLocY, -ShakePattern.MaxLocationOffsetClamp.Y, ShakePattern.MaxLocationOffsetClamp.Y);
    //OutLocationOffset.Z = FMath::Clamp(RawLocZ, -ShakePattern.MaxLocationOffsetClamp.Z, ShakePattern.MaxLocationOffsetClamp.Z);

    const float RotFreqMultiplier = ShakePattern.RotationFrequencyMultiplier;
    const float RotAmplMultiplier = ShakePattern.RotationAmplitudeMultiplier;
    const float RotTime = CurrentTime * RotFreqMultiplier;

    float RawPitch = NoiseGenerator.GetNoise(RotTime, 40.0f) * ShakePattern.RotationAmplitude.X * RotAmplMultiplier;
    float RawYaw = NoiseGenerator.GetNoise(RotTime, 50.0f) * ShakePattern.RotationAmplitude.Y * RotAmplMultiplier;
    float RawRoll = NoiseGenerator.GetNoise(RotTime, 60.0f) * ShakePattern.RotationAmplitude.Z * RotAmplMultiplier;

    // 회전 오프셋 제한 (Clamping) - FRotator가 Pitch, Yaw, Roll 순서라고 가정
    float ClampedPitch = FMath::Clamp(RawPitch, -ShakePattern.MaxRotationOffsetClamp.X, ShakePattern.MaxRotationOffsetClamp.X);
    float ClampedYaw = FMath::Clamp(RawYaw, -ShakePattern.MaxRotationOffsetClamp.Y, ShakePattern.MaxRotationOffsetClamp.Y);
    float ClampedRoll = FMath::Clamp(RawRoll, -ShakePattern.MaxRotationOffsetClamp.Z, ShakePattern.MaxRotationOffsetClamp.Z);

    OutRotationOffset = FRotator(ClampedPitch, ClampedYaw, ClampedRoll);
}

void UCameraShakeBase::InitializeNoiseGenerator()
{
    NoiseGenerator.SetNoiseType(ShakePattern.NoiseType);
    NoiseGenerator.SetSeed(ShakePattern.NoiseSeed);
    NoiseGenerator.SetFrequency(ShakePattern.NoiseFrequency);

    NoiseGenerator.SetFractalType(ShakePattern.NoiseFractalType);
    NoiseGenerator.SetFractalOctaves(ShakePattern.NoiseOctaves);
    NoiseGenerator.SetFractalLacunarity(ShakePattern.NoiseLacunarity);
    NoiseGenerator.SetFractalGain(ShakePattern.NoiseGain);

    InitialSeed = ShakePattern.NoiseSeed;
}
