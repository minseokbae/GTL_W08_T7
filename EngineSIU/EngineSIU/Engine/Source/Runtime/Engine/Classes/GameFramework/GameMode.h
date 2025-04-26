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
    AGameMode() = default;
    virtual void Tick(float DeltaTime) override;

private:
    // TODO : 원레는 Class를 등록해두고 찾는 형식일 듯 
    UClass* Controller = APlayerController::StaticClass();
    UClass* DefaultPawn = ADefaultPawn::StaticClass();

public:
    UClass* GetController() { return Controller; }
    void SetControlloer(UClass* NewController) { Controller = NewController; }
    UClass* GetDefaultPawn() { return DefaultPawn; }
    void SetDefaultPawn(UClass* pawn) { DefaultPawn = pawn; }
    
};
