#pragma once
#include "Controller.h"

class APlayerCameraManager;
class UPlayer;

class UCameraShakeBase;

class APlayerController : public AController
{
    DECLARE_CLASS(APlayerController, AController)
public:
    APlayerController();
    ~APlayerController();

    virtual void Tick(float DeltaTime) override;
    virtual void Input() override;
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void Initialize() override;
    void InitMouseLook();
    void SpawnPlayerCameraManager();
public:
    UPlayer* Player;
    POINT MouseCenterPos;
    
    bool btest = false;

    float MouseSens = 0.01f;

    UCameraShakeBase* CameraShakeModifier;
    
    APlayerCameraManager* GetPlayerCameraManager() { return PlayerCameraManager; }

private:
    // Just for game-jam
    bool bIsRunning;

    void HandleKeyDown(const FKeyEvent& InKeyEvent);
    void HandleKeyUp(const FKeyEvent& InKeyEvent);

    //void HandleMouseMove(const FPointerEvent& InMouseEvent);

    FSlateAppMessageHandler* MyMessageHandler;
    APlayerCameraManager* PlayerCameraManager;
};
