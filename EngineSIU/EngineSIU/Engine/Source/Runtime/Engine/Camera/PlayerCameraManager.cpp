#include "PlayerCameraManager.h"

#include "CameraComponent.h"
#include "CameraModifier.h"
#include "GameFramework/PlayerController.h"

#include "World/World.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/FLoaderOBJ.h"
#include <Engine/Engine.h>


APlayerCameraManager::APlayerCameraManager()
{
    TransformComponent = AddComponent<USceneComponent>("TransformComponent_0");
    RootComponent = TransformComponent;
}

APlayerCameraManager::~APlayerCameraManager()
{
}

void APlayerCameraManager::InitializeFor(APlayerController* PC)
{
    PCOwner = PC;
}

void APlayerCameraManager::Tick(float DeltaTime)
{
    AActor::Tick(DeltaTime);

    UCameraComponent* Camera =nullptr;
    for ( auto Component :  ViewTarget.Target->GetComponents())
    {
        if (Cast<UCameraComponent>(Component) != nullptr)
        {
            Camera = Cast<UCameraComponent>(Component);
        }
    }
    if (!Camera)
        return;
    FVector CurLocation = Camera->GetRelativeLocation();
    FRotator CurRotation = Camera->GetRelativeRotation();
    float CurFOV = Camera->GetFieldOfView();
    float NewFOV = 0;
    FVector NewLocation;
    FRotator NewRotation;
    static int cube;
    for (auto modifier : ModifierList)
    {
        if (!modifier->IsDisabled())
        {
            cube = 0;
            Camera->GetOwner()->SetActorRotation(FRotator(0, 0, 0));
            modifier->ModifyCamera(DeltaTime, CurLocation, CurRotation, CurFOV,
            NewLocation,NewRotation,NewFOV);
            Camera->SetRelativeLocation(NewLocation);
            Camera->SetRelativeRotation(NewRotation);
            Camera->SetFieldOfView(NewFOV);
        }
        else
        {
            if (cube == 0)
            {
                AStaticMeshActor* CubeActor = GEngine->ActiveWorld->SpawnActor<AStaticMeshActor>();
                CubeActor->GetStaticMeshComponent()->SetStaticMesh(FManagerOBJ::GetStaticMesh(L"Contents/Madara/Madara_Uchiha.obj"));
                CubeActor->SetActorLocation(Camera->GetWorldLocation());
                CubeActor->SetActorRotation(Camera->GetWorldRotation());
                cube += 1;
            }
        }
    }
}

void APlayerCameraManager::AddCameraModifier(UCameraModifier* CameraModifier)
{
    ModifierList.AddUnique(CameraModifier);
}
