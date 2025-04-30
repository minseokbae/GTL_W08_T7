#pragma once
#include "CameraModifier.h"

class UCameraFadeOutModifier : public UCameraModifier
{
    DECLARE_CLASS(UCameraFadeOutModifier, UCameraModifier)
public:
    UCameraFadeOutModifier() : TargetDuration(1.0f){};

    void Initialize(float Duration);
    bool ModifyCamera(float DeltaTime, APlayerCameraManager* NewCameraManager) override;
private:
    float TargetDuration;
};
