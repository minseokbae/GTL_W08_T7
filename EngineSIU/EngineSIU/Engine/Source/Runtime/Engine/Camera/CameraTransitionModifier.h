#pragma once
#include "CameraModifier.h"

class UCameraTransitionModifier : public UCameraModifier
{
    DECLARE_CLASS(UCameraTransitionModifier, UCameraModifier)

public:
    UCameraTransitionModifier();
    void Initialize(FVector Loc, FRotator Rot, float FOV, float Duration);

private:
    float ModifyDuration;
    FVector TargetLocation;
    FRotator TargetRotation;
    float TargetFOV;
    float ElapsedTime;
    float LocationSpeed;
    float RotationSpeed;
    bool bSpeedInitialized = false;

public:
    virtual bool ModifyCamera(float DeltaTime, APlayerCameraManager* NewCameraManager) override;
};
