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
    FVector GetScaledBoxExtent() const;
    FVector GetUnscaledBoxExtent() const;
};

