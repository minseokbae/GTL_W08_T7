#include "Player.h"

#include "GameFramework/PlayerController.h"

void UPlayer::SwitchController(APlayerController* PC)
{
    if (PlayerController)
        PlayerController= nullptr;
    PC->Player = this;
    PlayerController = PC; 
}
