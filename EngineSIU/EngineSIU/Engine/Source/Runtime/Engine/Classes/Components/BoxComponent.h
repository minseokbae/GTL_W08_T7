#pragma once
#include "ShapeComponent.h"

class UBoxComponent : UShapeComponent
{
    DECLARE_CLASS(UBoxComponent, UShapeComponent)
public:
    UBoxComponent();

protected:
    FVector BoxExtent;

public:
    void SetBoxExtent(FVector InBoxExtent);
    bool IsZeroExtent() const;
    FVector GetSclaedBoxExtent() const;
    FVector GetUnscaledBoxExtent() const;
};

void UBoxComponent::SetBoxExtent(FVector InBoxExtent)
{
    BoxExtent = InBoxExtent;
}

bool UBoxComponent::IsZeroExtent() const
{
    return BoxExtent.IsZero();
}

FVector UBoxComponent::GetSclaedBoxExtent() const
{
    return BoxExtent * GetWorldScale3D();
}

FVector UBoxComponent::GetUnscaledBoxExtent() const
{
    return BoxExtent;
}
