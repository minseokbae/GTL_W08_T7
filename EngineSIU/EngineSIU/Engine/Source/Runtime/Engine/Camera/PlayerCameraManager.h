#pragma once
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"

class UCameraModifier;

struct FViewTarget
{
    FViewTarget() : Target(nullptr){}
public:
    AActor* Target;

public:
    void SetNewTarget(AActor* NewTarget);
};

class APlayerCameraManager : public AActor
{
    DECLARE_CLASS(APlayerCameraManager, AActor)
public:
    APlayerCameraManager();
    virtual ~APlayerCameraManager();
    void InitializeFor(APlayerController* PC);

    void SetViewTarget(AActor* NewTarget) { ViewTarget.SetNewTarget(NewTarget); };
public:
    FLinearColor FadeColor;
    float FadeAmount;
    FVector2D FadeAlpha;
    float FadeTime;
    float FadeTimeRemaining;

    FName CameraStyle;
    struct FViewTarget ViewTarget;
    
protected:
    TArray<UCameraModifier*> ModifierList;
    APlayerController* PCOwner;
    
private:
    USceneComponent* TransformComponent;
};
