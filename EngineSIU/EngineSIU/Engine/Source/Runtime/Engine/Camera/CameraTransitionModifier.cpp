#include "CameraTransitionModifier.h"
#include "Math/JungleMath.h"
#include <Engine/EditorEngine.h>
#include "Engine/GameInstance.h"

UCameraTransitionModifier::UCameraTransitionModifier()
{
}

void UCameraTransitionModifier::Initialize(FVector Loc, FRotator Rot, float FOV, float Duration)
{
    TargetFOV = FOV;
    TargetLocation = Loc;
    TargetRotation = Rot;
    ModifyDuration = Duration;
    ElapsedTime = 0.0f;
}

void UCameraTransitionModifier::ModifyCamera(float DeltaTime, FVector ViewLocation, FRotator ViewRotation, float FOV, FVector& NewViewLocation, FRotator& NewViewRotation, float& NewFOV)
{
    UEditorEngine* Engine = Cast < UEditorEngine>(GEngine);
    Engine->SetCurrentController(Engine->GetEditorPlayer());
    ElapsedTime += DeltaTime;
    NewFOV = FMath::Lerp(FOV, TargetFOV, ElapsedTime / ModifyDuration);
    float LocationSpeed = (TargetLocation - ViewLocation).Length() / ModifyDuration;
    float RotationSpeed = FMath::RadiansToDegrees((TargetRotation - ViewRotation).ToVector().Length()) / ModifyDuration;
    NewViewLocation = JungleMath::FInterpTo(ViewLocation, TargetLocation, DeltaTime, LocationSpeed);
    NewViewRotation = JungleMath::RInterpTo(ViewRotation, TargetRotation, DeltaTime, RotationSpeed);
    if (ElapsedTime >= ModifyDuration)
    {
        DisableModifier();
        Engine->SetCurrentController(Engine->GetGameInstance()->GetLocalPlayer()->GetPlayerController());
    }
}

