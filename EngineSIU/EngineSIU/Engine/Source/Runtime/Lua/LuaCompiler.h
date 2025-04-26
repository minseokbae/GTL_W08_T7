#pragma once
#include "LuaInstance.h"
#include "HAL/PlatformType.h"
#include <memory>

class AActor;

class FLuaCompiler
{
public:
    FLuaCompiler();

    void Bind(AActor* Actor);

    void UnBind(AActor* Actor);

    void BeginPlay();

    void EndPlay();

    void Tick(float DeltaTime);

    void UpdateInput();
    
private:
    sol::state Lua;
    sol::table Input;
    std::unordered_map<uint32, std::unique_ptr<FLuaInstance>> LuaInstances;
};
