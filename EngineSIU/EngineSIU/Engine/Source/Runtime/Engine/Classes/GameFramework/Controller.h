#pragma once
#include "Actor.h"

class APawn;

class AController : public AActor
{
    DECLARE_CLASS(AController, AActor)
public:
    AController() = default;

    virtual void Tick(float DeltaTime) override;
    virtual void Input();

protected:
    APawn* PossessingPawn;

public:
    void SetPossessingPawn(APawn* Pawn){ PossessingPawn=Pawn; }
    APawn* GetPossessingPawn(){ return PossessingPawn; }
};
