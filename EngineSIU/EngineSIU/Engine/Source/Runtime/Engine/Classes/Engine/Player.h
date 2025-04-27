#pragma once
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

class APlayerController;

class UPlayer : public UObject
{
    DECLARE_CLASS(UPlayer, UObject)
public:
    UPlayer() = default;

protected:
    APlayerController* PlayerController;
public:
    APlayerController* GetPlayerController() { return PlayerController; }
    void SwitchController(APlayerController* PC);
};
