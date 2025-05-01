#pragma once
#include "CameraModifier.h"

class UCameraFadeInModifier : public UCameraModifier
{
    DECLARE_CLASS(UCameraFadeInModifier, UCameraModifier)
public:
    UCameraFadeInModifier(): TargetColor(FLinearColor::White), TargetDuration(1.0f) {};
    ~UCameraFadeInModifier();
    void Initialize(FLinearColor Color, float Duration) { TargetColor = Color; TargetDuration = Duration; }
    virtual bool ModifyCamera(float DeltaTime, APlayerCameraManager* NewCameraManager);

private:
    FLinearColor TargetColor;
    float TargetDuration;
    float SumTime= 0.0;
};
