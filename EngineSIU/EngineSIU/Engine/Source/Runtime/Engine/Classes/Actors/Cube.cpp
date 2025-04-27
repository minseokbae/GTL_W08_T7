#include "Cube.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/AssetManager.h"

#include "Engine/FLoaderOBJ.h"

#include "GameFramework/Actor.h"

ACube::ACube()
{
    // Begin Test
    //StaticMeshComponent->SetStaticMesh(FManagerOBJ::GetStaticMesh(L"Contents/helloBlender.obj"));
    StaticMeshComponent->SetStaticMesh(FManagerOBJ::GetStaticMesh(L"Contents/Reference/Reference.obj"));
    // End Test
    StaticMeshComponent2 = AddComponent<UStaticMeshComponent>("TestStaticMesh");
    StaticMeshComponent2->SetStaticMesh(FManagerOBJ::GetStaticMesh(L"Contents/Reference/Reference.obj"));
    StaticMeshComponent2->SetRelativeLocation(FVector(-5.0,0.0f,0.0f));
    StaticMeshComponent2->SetupAttachment(RootComponent);
}
void ACube::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    //SetActorRotation(GetActorRotation() + FRotator(0, 0, 1));

}
