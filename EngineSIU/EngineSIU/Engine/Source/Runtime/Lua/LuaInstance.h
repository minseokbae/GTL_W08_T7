#pragma once
#define SOL_LUAJIT 1
#include <sol/sol.hpp>
#include <lua.hpp>
#include "Container/String.h"

class USceneComponent;

class FLuaInstance
{
private:
    sol::environment Env;
    sol::function TickFunc;
public:
    FLuaInstance(sol::state& Lua, USceneComponent* Comp, FString FilePath);

    void Tick(float DeltaTime);
};
