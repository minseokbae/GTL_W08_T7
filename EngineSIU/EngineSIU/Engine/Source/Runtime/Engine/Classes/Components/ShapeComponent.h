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

    virtual UObject* Duplicate(UObject* InOuter) override;

    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

    FColor ShapeColor;

    bool bDrawOnlyIfSelected;

    FComponentOverlapSignature OnComponentBeginOverlap;
    FComponentOverlapSignature OnComponentEndOverlap;

    virtual void NotifyBeginOverlap(UShapeComponent* OverlappedComponent, AActor* OtherActor, UShapeComponent* OtherComp);
    virtual void NotifyEndOverlap(UShapeComponent* OverlappedComponent, AActor* OtherActor, UShapeComponent* OtherComp);
    //AActor* GetOwner() const;
};
