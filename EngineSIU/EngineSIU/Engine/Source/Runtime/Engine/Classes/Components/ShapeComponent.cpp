#include "Components/ShapeComponent.h"
#include "GameFramework/Actor.h"

UShapeComponent::UShapeComponent()
{
    ShapeColor = FColor(223, 149, 157, 255);
    bDrawOnlyIfSelected = false;
}

UShapeComponent::~UShapeComponent()
{
    
}

UObject* UShapeComponent::Duplicate(UObject* InOuter)
{
    ThisClass* NewComponent = Cast<ThisClass>(Super::Duplicate(InOuter));
    return NewComponent;
}

void UShapeComponent::BeginPlay()
{
    Super::BeginPlay();
    FEngineLoop::CollisionMgr.RegisterComponent(this);
}

void UShapeComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    // 파이 꺼질때 이게 호출이 안됌.
    FEngineLoop::CollisionMgr.UnregisterComponent(this);
}
    
// primitive에 대해서 하는게 맞는거 같긴 한데
void UShapeComponent::NotifyBeginOverlap(UShapeComponent* OverlappedComponent, AActor* OtherActor, UShapeComponent* OtherComp)
{
    UE_LOG(ELogLevel::Display, "Begin Overlap");
    OnComponentBeginOverlap.Broadcast(OverlappedComponent, OtherActor, OtherComp);
}

void UShapeComponent::NotifyEndOverlap(UShapeComponent* OverlappedComponent, AActor* OtherActor, UShapeComponent* OtherComp)
{
    UE_LOG(ELogLevel::Display, "End Overlap");
    OnComponentEndOverlap.Broadcast(OverlappedComponent, OtherActor, OtherComp);
}
