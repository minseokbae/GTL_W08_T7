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

    void GetProperties(TMap<FString, FString>& OutProperties) const override;

    void SetProperties(const TMap<FString, FString>& InProperties) override;

    void SetBoxExtent(FVector InBoxExtent);
    bool IsZeroExtent() const;
    FVector GetScaledBoxExtent() const;
    FVector GetUnscaledBoxExtent() const;
};

