#pragma once
#include "Math/Transform.h"
#include "CameraModifier.h"
#include "ThirdParty/FastNoiseLite/FastNoiseLite.h"

struct FCameraShakePattern
{
    //float LocationAmplitudeMultiplier = 0.5f;
    //float LocationFrequencyMultiplier = 0.5f;
    //FVector LocationAmplitude = FVector(0.05f, 0.05f, 0.05f);

    float RotationAmplitudeMultiplier = 1.f;
    float RotationFrequencyMultiplier = 10.f;
    FVector RotationAmplitude = FVector(1.f, 1.f, 1.f);

    // 일단 Perlin Noise Camera Shake Pattern을 기본으로 잡고 한다.
    float NoiseFrequency = 0.2f;
    int NoiseSeed = 1337;
    FastNoiseLite::NoiseType NoiseType = FastNoiseLite::NoiseType_Perlin;
    FastNoiseLite::FractalType NoiseFractalType = FastNoiseLite::FractalType_FBm;
    int NoiseOctaves = 3;
    float NoiseLacunarity = 2.0f;
    float NoiseGain = 0.3f;

    //FVector MaxLocationOffsetClamp = FVector(0.05f, 0.05f, 0.05f); 
    FVector MaxRotationOffsetClamp = FVector(1.0f, 1.0f, 1.0f);
};

class UCameraShakeBase : public UCameraModifier
{
    DECLARE_CLASS(UCameraShakeBase, UCameraModifier)

public:
    UCameraShakeBase();

    FCameraShakePattern ShakePattern;
    float Duration = -1.0f;
    float BlendInTime = 0.1f;
    float BlendOutTime = 0.2f;

private:
    float ElapsedTime = 0.f;
    bool bIsFinished = false;
    bool bIsActive = true;

    FastNoiseLite NoiseGenerator;
    int InitialSeed;

public:
    virtual void PlayShake();
    virtual void StopShake(bool bImmediate = true);

    float GetBlendWeight() const;
    bool IsFinished() const { return bIsFinished; }
    bool IsActive() const { return bIsActive; }

    virtual bool ModifyCamera(float DeltaTime, APlayerCameraManager* NewCameraManager) override;

protected:
    void InitializeNoiseGenerator();
    virtual void CalculateShakeOffsets(float CurrentTime, FVector& OutLocationOffset, FRotator& OutRotationOffset);

};
