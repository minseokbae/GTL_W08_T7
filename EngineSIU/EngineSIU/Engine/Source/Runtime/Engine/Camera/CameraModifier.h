#pragma once
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

class APlayerCameraManager;

class UCameraModifier : public UObject
{
    DECLARE_CLASS(UCameraModifier, UObject)
public:
    UCameraModifier();

    
    uint8 Priority;
protected:
    APlayerCameraManager* CameraOwner;

    float AlphaInTime;
    float AlphaOutTime;
    float Alpha;
    uint32 bDisabled;

public:
    APlayerCameraManager* GetCameraManager() { return CameraOwner; }
    virtual void AddedToCamera(APlayerCameraManager* Camera);
    void DisableModifier();
    void EnableModifier();
    virtual void ModifyCamera(float DeltaTime, FVector ViewLocation, FRotator ViewRotation, float FOV, FVector& NewViewLocation, FRotator& NewViewRotation, float& NewFOV);
};


