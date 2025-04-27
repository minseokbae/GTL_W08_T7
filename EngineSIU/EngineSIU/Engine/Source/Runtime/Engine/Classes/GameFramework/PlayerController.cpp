#include "PlayerController.h"

#include "Pawn.h"

APlayerController::APlayerController()
{

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
        Pawn->SetActorLocation(Pawn->GetActorLocation() + Pawn->GetActorForwardVector() * 0.1f);
    }
    if (GetAsyncKeyState('S') & 0x8000)
    {
        Pawn->SetActorLocation(Pawn->GetActorLocation() - Pawn->GetActorForwardVector() * 0.1f);
    }
    if (GetAsyncKeyState('A') & 0x8000)
    {
        Pawn->SetActorLocation(Pawn->GetActorLocation() - Pawn->GetActorRightVector() * 0.1f);
    }
    if (GetAsyncKeyState('D') & 0x8000)
    {
        Pawn->SetActorLocation(Pawn->GetActorLocation() + Pawn->GetActorRightVector() * 0.1f);
    }
    // if (GetAsyncKeyState(VK_LBUTTON) & 0x8000))
}
