#include "CameraTransitionModifier.h"
#include "Math/JungleMath.h"
#include <Engine/EditorEngine.h>
#include "Engine/GameInstance.h"
#include <UnrealEd/ImGuiBezierWidget.h>

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
    ElapsedTime += DeltaTime;
    if (GEngine->bUseBezier)
    {
        float BezierValue = ImGui::BezierValue(ElapsedTime / ModifyDuration, BezierCurve);
        NewFOV = FMath::Lerp(FOV, TargetFOV, BezierValue);
        NewViewLocation = FMath::Lerp(ViewLocation, TargetLocation, BezierValue);
        NewViewRotation = FRotator(FQuat::Slerp(ViewRotation.ToQuaternion(), TargetRotation.ToQuaternion(), BezierValue));
    }
    else
    {
        NewFOV = FMath::Lerp(FOV, TargetFOV, ElapsedTime / ModifyDuration);
        float LocationSpeed = (TargetLocation - ViewLocation).Length() / ModifyDuration;
        float RotationSpeed = FMath::RadiansToDegrees((TargetRotation - ViewRotation).ToVector().Length()) / ModifyDuration;
        NewViewLocation = JungleMath::FInterpTo(ViewLocation, TargetLocation, DeltaTime, LocationSpeed);
        NewViewRotation = JungleMath::RInterpTo(ViewRotation, TargetRotation, DeltaTime, RotationSpeed);
    }
    if (ElapsedTime >= ModifyDuration)
    {
        DisableModifier();
    }
}

