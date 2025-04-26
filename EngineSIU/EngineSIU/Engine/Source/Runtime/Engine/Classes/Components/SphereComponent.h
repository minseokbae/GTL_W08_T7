#pragma once
#include "Components/ShapeComponent.h"

class USphereComponent : UShapeComponent
{
    DECLARE_CLASS(USphereComponent, UShapeComponent)
public:
    USphereComponent();

protected:
    float SphereRadius;
    
    void SetSphereRadius(float InSphereRadius);
    float GetSclaedShpereRadius() const;
    float GetUnscaledSphereRadius() const;
    float GetShapeScale() const;
};

inline void USphereComponent::SetSphereRadius(float InSphereRadius)
{
    SphereRadius = InSphereRadius;
}

inline float USphereComponent::GetSclaedShpereRadius() const
{
    return SphereRadius * GetShapeScale();
}

inline float USphereComponent::GetUnscaledSphereRadius() const
{
    return SphereRadius;
}
inline float USphereComponent::GetShapeScale() const
{
    return std::min(std::min(GetWorldScale3D().X, GetWorldScale3D().Y), GetWorldScale3D().Z);
}
