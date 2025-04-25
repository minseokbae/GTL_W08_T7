#pragma once
#define SOL_LUAJIT 1
#include <sol/sol.hpp>
#include <lua.hpp>

class USceneComponent;

class FLuaInstance
{
public:
    sol::environment Env;
    sol::function TickFunc;

    FLuaInstance(sol::state& Lua, USceneComponent* Comp);

    void Tick(float DeltaTime);
};
