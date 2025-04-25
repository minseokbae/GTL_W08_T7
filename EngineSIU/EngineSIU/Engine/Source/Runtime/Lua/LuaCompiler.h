#pragma once
#include "LuaInstance.h"

class USceneComponent;

class FLuaCompiler
{
public:
    FLuaCompiler();

    void Bind(USceneComponent* Comp);

    void UnBind();

    void Tick(float DeltaTime);

    void SetTickFunc(sol::function newTick);

    
private:
    sol::state Lua;
    sol::environment SolEnv;
    sol::function TickFunc;
    USceneComponent* BindedComp = nullptr;
};
