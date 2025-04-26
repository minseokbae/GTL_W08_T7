#include "Components/ShapeComponent.h"

UShapeComponent::UShapeComponent()
{
    ShapeColor = FColor(223,149,157,255);
    bDrawOnlyIfSelected = false;
}

// primitive에 대해서 하는게 맞는거 같긴 한데; 아
void UShapeComponent::NotifyBeginOverlap(UShapeComponent* OverlappedComponent, AActor* OtherActor, UShapeComponent* OtherComp)
{
    OnComponentBeginOverlap.Broadcast(OverlappedComponent, OtherActor, OtherComp);
}

void UShapeComponent::NotifyEndOverlap(UShapeComponent* OverlappedComponent, AActor* OtherActor, UShapeComponent* OtherComp)
{
    OnComponentEndOverlap.Broadcast(OverlappedComponent, OtherActor, OtherComp);
}
