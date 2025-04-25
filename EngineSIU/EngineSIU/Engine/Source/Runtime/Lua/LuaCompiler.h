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

    void Tick(float DeltaTime);
    
private:
    sol::state Lua;
    std::unordered_map<uint32, std::unique_ptr<FLuaInstance>> LuaInstances;
};
