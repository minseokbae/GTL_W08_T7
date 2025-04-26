#pragma once
#define SOL_LUAJIT 1
#include <sol/sol.hpp>
#include <lua.hpp>
#include "Container/String.h"
#include <filesystem>

class AActor;

class USceneComponent;

class FLuaInstance
{
private:
    sol::environment Env;
    sol::function TickFunc;
    sol::function BeginPlayFunc;
    sol::function EndPlayFunc;
    std::string ScriptFile;
    std::filesystem::file_time_type LastWriteTime;
    AActor* BindedActor = nullptr;
public:
    FLuaInstance(sol::state& Lua, USceneComponent* Comp, FString FilePath);
    std::string GetScriptFile() { return ScriptFile; }
    std::filesystem::file_time_type GetLastWriteTime() { return LastWriteTime; }
    void Tick(float DeltaTime);
    void BeginPlay();
    void EndPlay();
    void Reload(sol::state& Lua);
    AActor* GetBindedActor() { return BindedActor; }
};
