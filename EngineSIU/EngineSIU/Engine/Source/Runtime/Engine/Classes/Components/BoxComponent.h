#pragma once
#include "ShapeComponent.h"

class UBoxComponent : public UShapeComponent
{
    DECLARE_CLASS(UBoxComponent, UShapeComponent)
public:
    UBoxComponent();

protected:
    FVector BoxExtent;

public:
    virtual UObject* Duplicate(UObject* InOuter) override;

    void SetBoxExtent(FVector InBoxExtent);
    bool IsZeroExtent() const;
    FVector GetScaledBoxExtent() const;
    FVector GetUnscaledBoxExtent() const;
};

