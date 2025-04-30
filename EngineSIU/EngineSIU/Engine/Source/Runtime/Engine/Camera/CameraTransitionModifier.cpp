#include "CameraTransitionModifier.h"
#include "Math/JungleMath.h"
#include <Engine/EditorEngine.h>
#include "Engine/GameInstance.h"
#include <UnrealEd/ImGuiBezierWidget.h>
#include "PlayerCameraManager.h"

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

bool UCameraTransitionModifier::ModifyCamera(float DeltaTime, APlayerCameraManager* NewCameraManager)
{
    ElapsedTime += DeltaTime;
    UCameraComponent* CachedCamera = NewCameraManager->GetCachedCamera();
    if (GEngine->bUseBezier)
    {
        float BezierValue = ImGui::BezierValue(ElapsedTime / ModifyDuration, BezierCurve);
        CachedCamera->SetFieldOfView(FMath::Lerp(CachedCamera->GetFieldOfView(), TargetFOV, BezierValue));
        CachedCamera->SetRelativeLocation(FMath::Lerp(CachedCamera->GetRelativeLocation(), TargetLocation, BezierValue));
        CachedCamera->SetRelativeRotation(FRotator(FQuat::Slerp(CachedCamera->GetRelativeRotation().ToQuaternion(), TargetRotation.ToQuaternion(), BezierValue)));
    }
    else
    {
        CachedCamera->SetFieldOfView(FMath::Lerp(CachedCamera->GetFieldOfView(), TargetFOV, ElapsedTime / ModifyDuration));
        if (!bSpeedInitialized)
        {
            LocationSpeed = (TargetLocation - CachedCamera->GetRelativeLocation()).Length() / ModifyDuration;
            RotationSpeed = FMath::RadiansToDegrees((TargetRotation - CachedCamera->GetRelativeLocation()).ToVector().Length()) / ModifyDuration;
            bSpeedInitialized = true;
        }
        CachedCamera->SetRelativeLocation(JungleMath::FInterpTo(CachedCamera->GetRelativeLocation(), TargetLocation, DeltaTime, LocationSpeed));
        CachedCamera->SetRelativeRotation(JungleMath::RInterpTo(CachedCamera->GetRelativeLocation(), TargetRotation, DeltaTime, RotationSpeed));
    }
    if (ElapsedTime >= ModifyDuration)
        return true;
    return false;
}

