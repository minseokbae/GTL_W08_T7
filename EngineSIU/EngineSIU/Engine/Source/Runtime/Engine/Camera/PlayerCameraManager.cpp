#include "PlayerCameraManager.h"

#include "CameraComponent.h"
#include "CameraModifier.h"
#include "WindowsPlatformTime.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"

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
        if (Cam)
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

    // 반복 도중 추가에 불안전한 반복문
    // for (auto modifier : ModifierList)
    // {
    //     if (!modifier->IsDisabled())
    //     {
    //         if (modifier->ModifyCamera(DeltaTime, this))
    //         {
    //             RemoveModifier(modifier);   
    //         }
    //     }
    // }

    // 중간 추가에 안전한 반복문
    for (size_t ModifierNum = 0; ModifierNum < ModifierList.Num(); ModifierNum++)
    {
        if (!ModifierList[ModifierNum]->IsDisabled())
        {
            if (ModifierList[ModifierNum]->ModifyCamera(DeltaTime,this))
            {
                RemoveModifier(ModifierList[ModifierNum]);
            }
        }
    }
    
    for (auto finishedmodifier : FinishedModifier)
    {
        ModifierList.Remove(finishedmodifier);
    }
    FinishedModifier.Empty();
}
void APlayerCameraManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    for (auto modifier : ModifierList)
    {
        GUObjectArray.MarkRemoveObject(modifier);
    }
}
void APlayerCameraManager::AddCameraModifier(UCameraModifier* CameraModifier)
{
    CameraModifier->AddedToCamera(this);
    ModifierList.AddUnique(CameraModifier);
}

void APlayerCameraManager::RemoveModifier(UCameraModifier* Modifier)
{
    GUObjectArray.MarkRemoveObject(Modifier);
    FinishedModifier.AddUnique(Modifier);
}
