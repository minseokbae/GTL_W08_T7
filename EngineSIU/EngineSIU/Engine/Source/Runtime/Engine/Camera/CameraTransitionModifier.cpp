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
    if (!bInitialized)
    {
        StartLocation = CachedCamera->GetRelativeLocation();
        StartRotation = CachedCamera->GetRelativeRotation();
        bInitialized = true;
    }
    if (GEngine->bUseBezier)
    {
        float BezierValue = ImGui::BezierValue(ElapsedTime / ModifyDuration, BezierCurve);
        CachedCamera->SetFieldOfView(FMath::Lerp(CachedCamera->GetFieldOfView(), TargetFOV, BezierValue));
        CachedCamera->SetRelativeLocation(FMath::Lerp(StartLocation, TargetLocation, BezierValue));
        CachedCamera->SetRelativeRotation(FRotator(FQuat::Slerp(StartRotation.ToQuaternion(), TargetRotation.ToQuaternion(), BezierValue)));
    }
    else
    {
        CachedCamera->SetFieldOfView(FMath::Lerp(CachedCamera->GetFieldOfView(), TargetFOV, ElapsedTime / ModifyDuration));
        float LocationSpeed = (TargetLocation - CachedCamera->GetRelativeLocation()).Length() / ModifyDuration;
        float RotationSpeed = FMath::RadiansToDegrees((TargetRotation - CachedCamera->GetRelativeLocation()).ToVector().Length()) / ModifyDuration;
        CachedCamera->SetRelativeLocation(JungleMath::FInterpTo(CachedCamera->GetRelativeLocation(), TargetLocation, DeltaTime, LocationSpeed));
        CachedCamera->SetRelativeRotation(JungleMath::RInterpTo(CachedCamera->GetRelativeLocation(), TargetRotation, DeltaTime, RotationSpeed));
    }
    if (ElapsedTime >= ModifyDuration)
        return true;
    return false;
}
