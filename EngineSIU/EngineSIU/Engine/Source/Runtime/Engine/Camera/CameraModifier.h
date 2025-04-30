#pragma once

#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

class APlayerCameraManager;

class UCameraModifier : public UObject
{
    DECLARE_CLASS(UCameraModifier, UObject)
    //DECLARE_ABSTRACT_CLASS(UCameraModifier, UObject)

public:
    UCameraModifier();

    virtual void ModifyCamera(
        float DeltaTime, 
        FVector ViewLocation, 
        FRotator ViewRotation, 
        float FOV,
        FVector& NewViewLocation,
        FRotator& NewViewRotation,
        float& NewFOV
);

    void DisableModifier();
    void EnableModifier();

protected:
    APlayerCameraManager* CameraOwner = nullptr;

    float AlphaInTime;
    float AlphaOutTime;
    float Alpha;
    bool bDisabled = false;

    uint8 Priority;

private:

public:
    APlayerCameraManager* GetCameraManager() { return CameraOwner; }
    virtual void AddedToCamera(APlayerCameraManager* Camera);
    bool IsDisabled() { return bDisabled; }

};


