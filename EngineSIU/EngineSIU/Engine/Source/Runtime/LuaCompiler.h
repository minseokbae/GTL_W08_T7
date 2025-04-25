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

    void UnBind();

    void Tick(float DeltaTime);

    void SetTickFunc(sol::function newTick);

    sol::state Lua;
private:
    
    sol::environment SolEnv;
    sol::function TickFunc;
    USceneComponent* BindedComp = nullptr;
};
