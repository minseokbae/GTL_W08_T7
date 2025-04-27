#include "BoxComponent.h"

UBoxComponent::UBoxComponent()
{
    BoxExtent = FVector(10.0f, 10.0f, 10.0f);
}

void UBoxComponent::SetBoxExtent(FVector InBoxExtent)
{
    BoxExtent = InBoxExtent;
}

bool UBoxComponent::IsZeroExtent() const
{
    return BoxExtent.IsZero();
}

FVector UBoxComponent::GetScaledBoxExtent() const
{
    return BoxExtent * GetWorldScale3D();
}

FVector UBoxComponent::GetUnscaledBoxExtent() const
{
    return BoxExtent;
}
