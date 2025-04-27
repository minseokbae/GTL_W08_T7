#pragma once
#include "Controller.h"

class UPlayer;

class APlayerController : public AController
{
    DECLARE_CLASS(APlayerController, AController)
public:
    APlayerController();

    virtual void Tick(float DeltaTime) override;
    virtual void Input() override;
    virtual void BeginPlay() override;

    void InitMouseLook();
public:
    UPlayer* Player;
    POINT MouseCenterPos;
    float MouseSens = 0.01f;
};
