#pragma once
#include "LocalPlayer.h"
#include "GameFramework/GameMode.h"
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

class UGameInstance : public UObject
{
    DECLARE_CLASS(UGameInstance, UObject)
public:
    UGameInstance();

    void Init();
    void StartGameInstance();
private:
    ULocalPlayer* CurrentPlayer;
    AGameMode* GameMode;
public:
    ULocalPlayer* GetLocalPlayer(){ return CurrentPlayer; }
    
};
