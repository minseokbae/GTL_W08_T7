#include "PlayerCameraManager.h"

#include "CameraComponent.h"
#include "CameraModifier.h"
#include "GameFramework/PlayerController.h"

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
    for (auto modifier : ModifierList)
    {
        if (!modifier->IsDisabled())
        {
            modifier->ModifyCamera(DeltaTime, CurLocation, CurRotation, CurFOV,
            NewLocation,NewRotation,NewFOV);
            Camera->SetRelativeLocation(NewLocation);
            Camera->SetRelativeRotation(NewRotation);
            Camera->SetFieldOfView(NewFOV);
            std::cout << *Camera->GetWorldLocation().ToString() << std::endl;
        }
    }
}

void APlayerCameraManager::AddCameraModifier(UCameraModifier* CameraModifier)
{
    ModifierList.AddUnique(CameraModifier);
}
