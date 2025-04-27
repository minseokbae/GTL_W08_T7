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

void UShapeComponent::BeginPlay()
{
    Super::BeginPlay();
    FEngineLoop::CollisionMgr.RegisterComponent(this);
}

void UShapeComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    FEngineLoop::CollisionMgr.UnregisterComponent(this);
}
    
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
