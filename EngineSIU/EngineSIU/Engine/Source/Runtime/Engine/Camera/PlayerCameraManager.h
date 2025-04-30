#pragma once
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"

class UCameraComponent;
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
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    void SetViewTarget(AActor* NewTarget) { ViewTarget.SetNewTarget(NewTarget); }
    void AddCameraModifier(UCameraModifier* CameraModifier);
public:
    FLinearColor FadeColor = FLinearColor::Red;
    float FadeAmount; // 현제 페이드 정도 
    FVector2D FadeAlpha; // 퀵 페이드 페이드 전환용 
    float FadeTime;
    float FadeTimeRemaining;

    FLinearColor LetterBoxColor = FLinearColor::Black;
    float LetterBoxHeight = 0.05;
    float LetterBoxWidth = 0.05f;

    FLinearColor VignetteColor = FLinearColor::Red;
    float  VignetteIntensity = 0.9f; // 비네팅 강도 (0 = 없음, 1 = 최대)
    float  VignetteSmoothness = 0.5;// 비네팅 부드러움 (0.01~0.5 정도)
    FVector2D VignetteCenter = {0.5f,0.5f};    // 비네팅 중심 (0.5,0.5 = 화면 중앙)
    
    FName CameraStyle;
    struct FViewTarget ViewTarget;
    
protected:
    TArray<UCameraModifier*> ModifierList;
    TArray<UCameraModifier*> FinishedModifier;
    APlayerController* PCOwner;

    UCameraComponent* CachedCamera = nullptr;
private:
    USceneComponent* TransformComponent;

public:
    UCameraComponent* GetCachedCamera() { return CachedCamera; }
    void SetCachedCamera(UCameraComponent* NewCachedCamera) { CachedCamera = NewCachedCamera; }
    void RemoveModifier(UCameraModifier* Modifier);
};


