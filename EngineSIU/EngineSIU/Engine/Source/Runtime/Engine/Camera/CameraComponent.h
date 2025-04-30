#pragma once
#include "Components/SceneComponent.h"


class UCameraComponent : public USceneComponent
{
    DECLARE_CLASS(UCameraComponent, USceneComponent)

public:
    UCameraComponent() = default;

private:
    float FieldOfView = 60.0f;
    float AspectRatio = 1.0f;

public:
    float GetFieldOfView() const { return FieldOfView; }
    void SetFieldOfView(float InFieldOfView) { FieldOfView = InFieldOfView; }
    float GetAspectRatio() const { return AspectRatio; }
    void SetAspectRatio(float InAspectRatio) { AspectRatio = InAspectRatio; }
};
