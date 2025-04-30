#include "DefaultPawn.h"

#include "Engine/FLoaderOBJ.h"

ADefaultPawn::ADefaultPawn()
{
    StaticMeshComponent = AddComponent<UStaticMeshComponent>("StaticMeshComponent_0");
    RootComponent = StaticMeshComponent;
    StaticMeshComponent->SetStaticMesh(FManagerOBJ::GetStaticMesh(L"Contents/Reference/Reference.obj"));
    SpringArmComponent = AddComponent<USpringArmComponent>("SpringArmComponent");
    SpringArmComponent->SetupAttachment(RootComponent);
    StaticMeshComponent = AddComponent<UStaticMeshComponent>("StaticMeshComponent_1");
    StaticMeshComponent->SetStaticMesh(FManagerOBJ::GetStaticMesh(L"Contents/Cube/cube-tex.obj"));
    StaticMeshComponent->SetupAttachment(SpringArmComponent);
}
