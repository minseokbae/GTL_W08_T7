#include "CameraModifier.h"

UCameraModifier::UCameraModifier()
{
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
