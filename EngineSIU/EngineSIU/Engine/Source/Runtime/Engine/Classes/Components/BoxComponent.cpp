#include "BoxComponent.h"
#include <UObject/Casts.h>

UBoxComponent::UBoxComponent()
{
    BoxExtent = FVector(10.0f, 10.0f, 10.0f);
}

UObject* UBoxComponent::Duplicate(UObject* InOuter)
{
    ThisClass* NewComponent = Cast<ThisClass>(Super::Duplicate(InOuter));
    NewComponent->BoxExtent = BoxExtent;
    return NewComponent;
}

void UBoxComponent::GetProperties(TMap<FString, FString>& OutProperties) const
{
    Super::GetProperties(OutProperties);
    OutProperties.Add(TEXT("BoxExtent"), *BoxExtent.ToString());
}

void UBoxComponent::SetProperties(const TMap<FString, FString>& InProperties)
{
    Super::SetProperties(InProperties);
    const FString* TempStr = nullptr;
    TempStr = InProperties.Find(TEXT("BoxExtent"));
    if (TempStr)
    {
        BoxExtent.InitFromString(*TempStr);
    }
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
