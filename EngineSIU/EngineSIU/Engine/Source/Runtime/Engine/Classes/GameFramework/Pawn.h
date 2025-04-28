#pragma once
#include "Actor.h"

class AController;

class APawn : public AActor
{
    DECLARE_CLASS(APawn, AActor)
public:
    APawn();
    bool IsOverlapWall() { return bOverlapWall; }
    void SetOverlapWall(bool value) { bOverlapWall = value; }

private:
    AController* PossessedController;
    bool bOverlapWall = false;

public:
    void SetPossessedController(AController* NewController) { PossessedController = NewController; }
};
