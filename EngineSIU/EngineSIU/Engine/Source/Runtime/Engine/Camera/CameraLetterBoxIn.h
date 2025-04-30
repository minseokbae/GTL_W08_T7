#pragma once
#include "CameraModifier.h"

class UCameraLetterBoxIn : public UCameraModifier
{
    DECLARE_CLASS(UCameraLetterBoxIn, UCameraModifier)
public:
    UCameraLetterBoxIn() : TargetDuration(1.0f){}
    void Initialize(float Duration);
    bool ModifyCamera(float DeltaTime, APlayerCameraManager* NewCameraManager) override;

private:
    float TargetDuration;
    float Start =0.5f;
};
