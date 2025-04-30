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
    for (auto Component : ViewTarget.Target->GetComponents())
    {
        if (Cast<UCameraComponent>(Component) != nullptr)
        {
            Camera = Cast<UCameraComponent>(Component);
        }
    }

    if (!Camera) return;

    FVector CurrentLocation = Camera->GetRelativeLocation(); 
    FRotator CurrentRotation = Camera->GetRelativeRotation();
    float CurrentFOV = Camera->GetFieldOfView();

    FVector ModifiedLocation = CurrentLocation;
    FRotator ModifiedRotation = CurrentRotation;
    float ModifiedFOV = CurrentFOV;

    for (auto modifier : ModifierList)
    {
        if (modifier && !modifier->IsDisabled()) 
        {
            modifier->ModifyCamera(
                DeltaTime,
                ModifiedLocation, 
                ModifiedRotation, 
                ModifiedFOV,      
                ModifiedLocation, 
                ModifiedRotation, 
                ModifiedFOV       
            );
        }
    }
    Camera->SetRelativeLocation(ModifiedLocation);
    Camera->SetRelativeRotation(ModifiedRotation);
    Camera->SetFieldOfView(ModifiedFOV);


}

void APlayerCameraManager::AddCameraModifier(UCameraModifier* CameraModifier)
{
    CameraModifier->AddedToCamera(this);
    ModifierList.AddUnique(CameraModifier);
}
