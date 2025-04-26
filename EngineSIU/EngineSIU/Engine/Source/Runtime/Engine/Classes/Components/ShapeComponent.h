#pragma once
#include "PrimitiveComponent.h"
#include "Delegates/DelegateCombination.h"

DECLARE_MULTICAST_DELEGATE_ThreeParams(FComponentOverlapSignature, UShapeComponent* /*OverlappedComponent*/, AActor* /*OtherActor*/, UShapeComponent* /*OtherComp*/);

class UShapeComponent : public UPrimitiveComponent
{
    DECLARE_CLASS(UShapeComponent, UPrimitiveComponent)
public:
    UShapeComponent();
    ~UShapeComponent();

    FColor ShapeColor;

    bool bDrawOnlyIfSelected;

    FComponentOverlapSignature OnComponentBeginOverlap;
    FComponentOverlapSignature OnComponentEndOverlap;

    virtual void NotifyBeginOverlap(UShapeComponent* OverlappedComponent, AActor* OtherActor, UShapeComponent* OtherComp);
    virtual void NotifyEndOverlap(UShapeComponent* OverlappedComponent, AActor* OtherActor, UShapeComponent* OtherComp);
    //AActor* GetOwner() const;
};
