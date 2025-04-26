#include "PlayerController.h"

#include "Pawn.h"

APlayerController::APlayerController()
{
    TransformComponent = AddComponent<USceneComponent>("TransformComponent_0");
    RootComponent = TransformComponent;
}

void APlayerController::Tick(float DeltaTime)
{
    AController::Tick(DeltaTime);
}

void APlayerController::Input()
{
    AController::Input();

    if (GetAsyncKeyState('W') & 0x8000)
    {
        PossessingPawn->SetActorLocation(PossessingPawn->GetActorLocation() + FVector(.1f, 0.0f, 0.0f));
    }
    if (GetAsyncKeyState('S') & 0x8000)
    {
        PossessingPawn->SetActorLocation(PossessingPawn->GetActorLocation() + FVector(-.1f, 0.0f, 0.0f));
    }
    if (GetAsyncKeyState('A') & 0x8000)
    {
        PossessingPawn->SetActorLocation(PossessingPawn->GetActorLocation() + FVector(0.0f, -0.1f, 0.0f));
    }
    if (GetAsyncKeyState('D') & 0x8000)
    {
        PossessingPawn->SetActorLocation(PossessingPawn->GetActorLocation() + FVector(0.0f, 0.1f, 0.0f));
    }
}
