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
    void SetNewTarget(AActor* NewTarget) {Target = NewTarget;};
};

class APlayerCameraManager : public AActor
{
    DECLARE_CLASS(APlayerCameraManager, AActor)
public:
    APlayerCameraManager();
    virtual ~APlayerCameraManager();
    void InitializeFor(APlayerController* PC);
    virtual void Tick(float DeltaTime) override;

    void SetViewTarget(AActor* NewTarget) { ViewTarget.SetNewTarget(NewTarget); }
    void AddCameraModifier(UCameraModifier* CameraModifier);
public:
    FLinearColor FadeColor = FLinearColor::White;
    float FadeAmount; // 현제 페이드 정도 
    FVector2D FadeAlpha; // 퀵 페이드 페이드 전환용 
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
