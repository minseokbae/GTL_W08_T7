#pragma once
#include "Components/ShapeComponent.h"

class USphereComponent : public UShapeComponent
{
    DECLARE_CLASS(USphereComponent, UShapeComponent)
public:
    USphereComponent();

    virtual UObject* Duplicate(UObject* InOuter) override;

    void GetProperties(TMap<FString, FString>& OutProperties) const override;

    void SetProperties(const TMap<FString, FString>& InProperties) override;

    float GetScaledSphereRadius() const;
    float GetUnscaledSphereRadius() const;
    float GetShapeScale() const;
    void SetSphereRadius(float InSphereRadius);
    
protected:
    float SphereRadius;
};

inline void USphereComponent::SetSphereRadius(float InSphereRadius)
{
    SphereRadius = InSphereRadius;
}

inline float USphereComponent::GetScaledSphereRadius() const
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
