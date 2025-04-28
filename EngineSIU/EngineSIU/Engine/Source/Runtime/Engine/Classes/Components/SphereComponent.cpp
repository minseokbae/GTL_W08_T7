#include "SphereComponent.h"
#include <UObject/Casts.h>

USphereComponent::USphereComponent()
{
    SphereRadius = 1.0f;
}

UObject* USphereComponent::Duplicate(UObject* InOuter)
{
    ThisClass* NewComponent = Cast<ThisClass>(Super::Duplicate(InOuter));
    NewComponent->SphereRadius = SphereRadius;
    return NewComponent;
}
