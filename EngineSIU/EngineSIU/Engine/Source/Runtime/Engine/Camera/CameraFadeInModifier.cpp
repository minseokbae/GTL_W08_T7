#include "CameraFadeInModifier.h"

#include "CameraFadeOutModifier.h"
#include "PlayerCameraManager.h"

UCameraFadeInModifier::~UCameraFadeInModifier()
{
}

bool UCameraFadeInModifier::ModifyCamera(float DeltaTime, APlayerCameraManager* NewCameraManager)
{
    SumTime += DeltaTime;
    NewCameraManager->FadeColor = TargetColor;
    NewCameraManager->FadeAmount = SumTime/TargetDuration;
    if (SumTime > TargetDuration)
    {
        UCameraFadeOutModifier* FadeOutModifier = FObjectFactory::ConstructObject<UCameraFadeOutModifier>(this);
        FadeOutModifier->Initialize(3.0f);
        NewCameraManager->AddCameraModifier(FadeOutModifier);
        return true;
    }
    return false;
}
