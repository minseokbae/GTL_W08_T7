#pragma once
#include "Controller.h"

class APlayerController : public AController
{
    DECLARE_CLASS(APlayerController, AController)
public:
    APlayerController();

    virtual void Tick(float DeltaTime) override;
    virtual void Input() override;
public:
    USceneComponent* TransformComponent;
};
