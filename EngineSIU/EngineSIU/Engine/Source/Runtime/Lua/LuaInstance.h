#pragma once
#define SOL_LUAJIT 1
#include <sol/sol.hpp>
#include <lua.hpp>
#include "Container/String.h"
#include <filesystem>

class USceneComponent;

class FLuaInstance
{
private:
    sol::environment Env;
    sol::function TickFunc;
    std::string ScriptFile;
    std::filesystem::file_time_type LastWriteTime;
public:
    FLuaInstance(sol::state& Lua, USceneComponent* Comp, FString FilePath);
    std::string GetScriptFile() { return ScriptFile; }
    std::filesystem::file_time_type GetLastWriteTime() { return LastWriteTime; }
    void Tick(float DeltaTime);
    void Reload(sol::state& Lua);
};
