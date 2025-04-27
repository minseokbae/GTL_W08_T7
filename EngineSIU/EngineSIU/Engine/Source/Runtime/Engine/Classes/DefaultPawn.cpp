#include "DefaultPawn.h"

#include "Engine/FLoaderOBJ.h"

ADefaultPawn::ADefaultPawn()
{
    StaticMeshComponent = AddComponent<UStaticMeshComponent>("StaticMeshComponent_0");
    RootComponent = StaticMeshComponent;
    StaticMeshComponent->SetStaticMesh(FManagerOBJ::GetStaticMesh(L"Contents/Reference/Reference.obj"));
    CameraComponent = AddComponent<UCameraComponent>("CameraComponent");
    CameraComponent->SetRelativeLocation(FVector(-5.0f, 0.0f, 5.0f));
    CameraComponent->SetupAttachment(RootComponent);
}
