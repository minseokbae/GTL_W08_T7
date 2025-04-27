#include "Controller.h"

AController::AController()
{
    TransformComponent = AddComponent<USceneComponent>("TransformComponent_0");
    RootComponent = TransformComponent;
}

void AController::Tick(float DeltaTime)
{
    AActor::Tick(DeltaTime);
}

void AController::Input()
{
}
