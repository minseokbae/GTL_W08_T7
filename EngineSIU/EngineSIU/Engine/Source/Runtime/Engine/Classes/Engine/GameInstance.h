#pragma once
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

class UGameInstance : public UObject
{
    DECLARE_CLASS(UGameInstance, UObject)
public:
    UGameInstance();

    void Init();
};
