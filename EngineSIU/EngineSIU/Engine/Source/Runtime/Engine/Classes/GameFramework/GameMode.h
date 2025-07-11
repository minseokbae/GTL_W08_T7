#pragma once
#include "Actor.h"
#include "Controller.h"
#include "DefaultPawn.h"
#include "PlayerController.h"

class APawn;

class AGameMode : public AActor
{
    DECLARE_CLASS(AGameMode, AActor)
public:
    AGameMode();
    virtual void Tick(float DeltaTime) override;
    void StartGame();
    bool bGameOver = false;
    bool bWin = false;
private:
    UClass* Controller = APlayerController::StaticClass();
    UClass* DefaultPawn = ADefaultPawn::StaticClass();

    float Score = 0.0f;
    float WinScore = 5.0f;

    int32 HP =3;
protected:
    USceneComponent* TransformComponent = nullptr;

public:
    UClass* GetController() { return Controller; }
    void SetControlloer(UClass* NewController) { Controller = NewController; }
    UClass* GetDefaultPawn() { return DefaultPawn; }
    void SetDefaultPawn(UClass* pawn) { DefaultPawn = pawn; }
    
    int32 GetHP() { return HP; }
    float GetScore() { return Score; }
    void GameOver();
    void Win();
    void RestartGame();
    void AddScore(float InScore);
};
