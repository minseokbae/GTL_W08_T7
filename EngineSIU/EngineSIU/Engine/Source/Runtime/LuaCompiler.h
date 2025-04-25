#pragma once
#define SOL_LUAJIT 1
#include <sol/sol.hpp>
#include <lua.hpp>

class USceneComponent;

class FLuaCompiler
{
public:
    FLuaCompiler();

    void Bind(USceneComponent* Comp);

    void Tick(float DeltaTime);

private:
    sol::state Lua;
};
