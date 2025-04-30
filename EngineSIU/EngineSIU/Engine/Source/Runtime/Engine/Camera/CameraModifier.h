#pragma once

#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

class APlayerCameraManager;

class UCameraModifier : public UObject
{
    DECLARE_CLASS(UCameraModifier, UObject)
public:
    UCameraModifier();

    void DisableModifier();
    void EnableModifier();

    virtual bool ModifyCamera(float DeltaTime, APlayerCameraManager* NewCameraManager);

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


