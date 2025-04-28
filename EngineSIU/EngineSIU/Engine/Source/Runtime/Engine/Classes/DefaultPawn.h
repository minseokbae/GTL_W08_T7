#pragma once
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Pawn.h"

class ADefaultPawn : public APawn
{
    DECLARE_CLASS(ADefaultPawn, APawn)
public:
    ADefaultPawn();

public:
    UStaticMeshComponent* GetStaticMeshComponent() const { return StaticMeshComponent; }
protected:
    UStaticMeshComponent* StaticMeshComponent = nullptr;
    UCameraComponent* CameraComponent = nullptr;
};
