#pragma once
#include "Components/ShapeComponent.h"

class UCapsuleComponent : public UShapeComponent
{
    DECLARE_CLASS(UCapsuleComponent, UShapeComponent)
public:
    UCapsuleComponent();

protected:
    float CapsuleHalfHeight;
    float CapsuleRadius;

public:
    bool IsZeroExtent();
    float GetScaledCapsuleRadius();
    float GetScaledCapsuleHalfHeight();
    float GetUnscaledCapsuleRadius();
    float GetUnscaledCapsuleHalfHeight();
    void SetCapsuleRadius(float InCapsuleRadius);
    void SetCapsuleHalfHeight(float InCapsuleHalHeight);
};
inline bool UCapsuleComponent::IsZeroExtent()
{
    return (CapsuleRadius == 0.f) && (CapsuleHalfHeight == 0.f);
}

inline float UCapsuleComponent::GetScaledCapsuleRadius()
{
    const FVector ComponentScale = GetWorldScale3D();
    return CapsuleRadius * (ComponentScale.X < ComponentScale.Y ? ComponentScale.X : ComponentScale.Y);
}

inline float UCapsuleComponent::GetScaledCapsuleHalfHeight()
{
    return CapsuleHalfHeight * GetWorldScale3D().Z;
}

inline float UCapsuleComponent::GetUnscaledCapsuleRadius()
{
    return CapsuleRadius;
}

inline float UCapsuleComponent::GetUnscaledCapsuleHalfHeight()
{
    return CapsuleHalfHeight;
}

inline void UCapsuleComponent::SetCapsuleRadius(float InCapsuleRadius)
{
    CapsuleRadius = InCapsuleRadius;
}

inline void UCapsuleComponent::SetCapsuleHalfHeight(float InCapsuleHalHeight)
{
    CapsuleHalfHeight = InCapsuleHalHeight;
}
