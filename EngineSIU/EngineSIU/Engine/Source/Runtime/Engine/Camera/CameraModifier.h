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
    bool bDisabled = false;

public:
    APlayerCameraManager* GetCameraManager() { return CameraOwner; }
    virtual void AddedToCamera(APlayerCameraManager* Camera);
    bool IsDisabled() {return bDisabled;}
    void DisableModifier();
    void EnableModifier();
    virtual void ModifyCamera(float DeltaTime, APlayerCameraManager* NewCameraManager);
};


