﻿#pragma once
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
    UClass* Controller = APlayerController::StaticClass();
    UClass* DefaultPawn = ADefaultPawn::StaticClass();

public:
    UClass* GetController() { return Controller; }
    void SetControlloer(UClass* NewController) { Controller = NewController; }
    UClass* GetDefaultPawn() { return DefaultPawn; }
    void SetDefaultPawn(UClass* pawn) { DefaultPawn = pawn; }
    
};
