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

void USphereComponent::GetProperties(TMap<FString, FString>& OutProperties) const
{
    Super::GetProperties(OutProperties);
    OutProperties.Add(TEXT("SphereRadius"), FString::Printf(TEXT("%f"), SphereRadius));
}

void USphereComponent::SetProperties(const TMap<FString, FString>& InProperties)
{
    Super::SetProperties(InProperties);
    const FString* TempStr = nullptr;
    TempStr = InProperties.Find(TEXT("SphereRadius"));
    if (TempStr)
    {
        SphereRadius = FString::ToFloat(*TempStr);
    }
}
