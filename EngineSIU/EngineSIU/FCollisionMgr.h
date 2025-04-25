#pragma once
#include "Delegates/DelegateCombination.h"
DECLARE_MULTICAST_DELEGATE(FCollisionDelegate);

class FCollisionMgr
{
public:
    FCollisionDelegate OnCollision;
    void OnCollisionDetected()
    {
        OnCollision.Broadcast();
    }
};



