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
void UCameraModifier::ModifyCamera(float DeltaTime, FVector ViewLocation, FRotator ViewRotation, float FOV, FVector& NewViewLocation,
    FRotator& NewViewRotation, float& NewFOV) // New 계열이 아웃 파라미터
{

}
