#include "CameraModifier.h"
#include <Engine/Engine.h>

UCameraModifier::UCameraModifier()
{
    BezierCurve = GEngine->BezierCurve;
}

void UCameraModifier::AddedToCamera(APlayerCameraManager* Camera)
{
    CameraOwner = Camera;
}

void UCameraModifier::DisableModifier()
{
    bDisabled = true; 
}

void UCameraModifier::EnableModifier()
{
    bDisabled = false;
}

bool UCameraModifier::ModifyCamera(float DeltaTime, APlayerCameraManager* NewCameraManager)
{
    return true;
}
