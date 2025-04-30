#pragma once
#include "Actor.h"

class AController;

class APawn : public AActor
{
    DECLARE_CLASS(APawn, AActor)
public:
    APawn();

private:
    AController* PossessedController;

public:
    void SetPossessedController(AController* NewController) { PossessedController = NewController; }
};
