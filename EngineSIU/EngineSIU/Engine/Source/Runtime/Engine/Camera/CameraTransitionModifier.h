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

public:
    virtual void ModifyCamera(float DeltaTime, FVector ViewLocation, FRotator ViewRotation, float FOV, FVector& NewViewLocation, FRotator& NewViewRotation, float& NewFOV) override;
};
