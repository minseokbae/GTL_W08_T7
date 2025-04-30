#pragma once
#include "CameraModifier.h"

class UCameraLetterBoxOut : public UCameraModifier
{
    DECLARE_CLASS(UCameraLetterBoxOut, UCameraModifier)
public:
    UCameraLetterBoxOut() : TargetDuration(1.0f){};
    virtual bool ModifyCamera(float DeltaTime, APlayerCameraManager* NewCameraManager) override;
    void Initialize(float Duration);

private:
    float Start = 0.0f;
    float TargetDuration;
};
