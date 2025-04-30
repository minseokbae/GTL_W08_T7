#pragma once

#include "Components/SceneComponent.h"
#include "Math/Quat.h"

class USpringArmComponent : public USceneComponent
{
    DECLARE_CLASS(USpringArmComponent, USceneComponent)

    virtual UObject* Duplicate(UObject* InOuter) override;

    virtual void GetProperties(TMap<FString, FString>& OutProperties) const override;
    virtual void SetProperties(const TMap<FString, FString>& InProperties) override;

    USpringArmComponent();
    float TargetArmLength;
    FVector SocketOffset;
    FVector TargetOffset;
    bool bDoCollisionTest = true;
    bool bUsePawnControlRotation = true;
    bool bInheritPitch = true;
    bool bInheritYaw = true;
    bool bInheritRoll = true;

    bool bEnableCameraLag = true;
    float CameraLagSpeed;
    float CameraLagMaxDistance;

    bool bEnableCameraRotationLag = true;
    float CameraRotationLagSpeed;

    FVector PreviousDesiredLoc;
    FVector PreviousArmOrigin;
    FRotator PreviousDesiredRot;

    bool bIsCameraFixed = false;
    FVector UnfixedCameraPosition;

    FRotator GetDesiredRotation() const;
    FRotator GetTargetRotation() const;

    void UpdateDesiredArmLocation(bool bDoTrace, bool bDoLocationLag, bool bDoRotationLag, float DeltaTime);
    virtual void TickComponent(float DeltaTime) override;
protected:
    FVector RelativeSocketLocation;
    FQuat RelativeSocketRotation;

    FVector BlendLocations(const FVector& DesiredArmLocation, const FVector& TraceHitLocation, bool bHitSomething, float DeltaTime);
};
