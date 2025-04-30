#include "PlayerCameraManager.h"

#include "CameraComponent.h"
#include "CameraModifier.h"
#include "WindowsPlatformTime.h"
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
    AActor* TargetActor = Cast<AActor>(PC->GetPossessingPawn()); 
    if (TargetActor)
        SetViewTarget(TargetActor);
    else
    {
        UE_LOG(ELogLevel::Error, "PlayerController dont have any Posses pawn");
    }
    for (auto Comp : TargetActor->GetComponents())
    {
        UCameraComponent* Cam =Cast<UCameraComponent>(Comp);
        if ( Cam)
        {
            CachedCamera = Cam;
            break;
        }
    }
    if (CachedCamera == nullptr)
    {
        CachedCamera = TargetActor->AddComponent<UCameraComponent>("CameraComponent_0");
    }
}

void APlayerCameraManager::Tick(float DeltaTime)
{
    AActor::Tick(DeltaTime);

    static float CameraFadeTime = 0.0f;
    CameraFadeTime += DeltaTime;

    FadeAmount = FMath::Sin(CameraFadeTime) * .5f + 0.5f;
    
    FVector NewLocation = CachedCamera->GetRelativeLocation();
    FRotator NewRotation = CachedCamera->GetRelativeRotation();
    float NewFOV = CachedCamera->GetFieldOfView();
    for (auto modifier : ModifierList)
    {
        
        if (!modifier->IsDisabled())
        {
            modifier->ModifyCamera(DeltaTime,FVector::Zero(),FRotator(0,0,0),0,
            NewLocation,NewRotation,NewFOV);
            CachedCamera->SetRelativeLocation(NewLocation);
            CachedCamera->SetRelativeRotation(NewRotation);
            CachedCamera->SetFieldOfView(NewFOV);
        }
    }
}

void APlayerCameraManager::AddCameraModifier(UCameraModifier* CameraModifier)
{
    ModifierList.AddUnique(CameraModifier);
}
