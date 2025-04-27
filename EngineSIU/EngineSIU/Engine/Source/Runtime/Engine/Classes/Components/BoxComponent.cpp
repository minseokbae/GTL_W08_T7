#include "BoxComponent.h"

UBoxComponent::UBoxComponent()
{
    BoxExtent = FVector(1.0f, 1.0f, 1.0f);
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
