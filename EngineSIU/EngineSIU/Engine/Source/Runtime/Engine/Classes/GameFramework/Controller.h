#pragma once
#include "Actor.h"

class APawn;

class AController : public AActor
{
    DECLARE_CLASS(AController, AActor)
public:
    AController();

    virtual void Tick(float DeltaTime) override;
    virtual void Input();
    virtual void Initialize();

protected:
    APawn* Pawn;
    USceneComponent* TransformComponent;

public:
    void AttachtoPawn(APawn* InPawn){ Pawn= InPawn; }
    APawn* GetPossessingPawn(){ return Pawn; }
};
