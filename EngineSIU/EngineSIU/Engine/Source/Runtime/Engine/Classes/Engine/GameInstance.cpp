#include "GameInstance.h"

UGameInstance::UGameInstance()
{
}

void UGameInstance::Init()
{
    CurrentPlayer = FObjectFactory::ConstructObject<ULocalPlayer>(this);
}

void UGameInstance::StartGameInstance()
{
    
}
