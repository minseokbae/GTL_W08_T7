#include "BoxComponent.h"

UBoxComponent::UBoxComponent()
{
    BoxExtent = FVector(32.0f, 32.0f,32.0f);
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
