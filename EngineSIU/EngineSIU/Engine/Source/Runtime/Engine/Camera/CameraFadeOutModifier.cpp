#include "CameraFadeOutModifier.h"

#include "PlayerCameraManager.h"

void UCameraFadeOutModifier::Initialize(float Duration)
{
    TargetDuration = Duration;
}

bool UCameraFadeOutModifier::ModifyCamera(float DeltaTime, APlayerCameraManager* NewCameraManager)
{
    NewCameraManager->FadeAmount -= DeltaTime/ TargetDuration;
    if (NewCameraManager->FadeAmount <= 0.0f)
        return true;
    return false;
}
